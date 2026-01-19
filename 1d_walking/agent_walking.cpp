#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <unordered_map>


struct Agent {
    int id;
    int pos;
    int age;

    enum class AgentState {
        Alive,
        Dead,
        Finished
    };

    AgentState state;

    bool active() const {
        return state == AgentState::Alive;
    }

    void move(double p_forward, std::mt19937& rng) {
        std::bernoulli_distribution move_coin(0.5);

        if (move_coin(rng)) {
            bool forward = (std::generate_canonical<double, 10>(rng) < p_forward);
            pos += forward? 1:-1;
        }
    }

    void age_up(int tick) {
        age = tick/100;
    }

    bool try_die(std::mt19937& rng, int tick) {
        if (tick%100!=0) return false;
        
        constexpr double alpha = 0.0001;
        constexpr double beta = 0.08;

        static std::vector<double> gompertz_cache;
        if(age>=gompertz_cache.size()){
            for(int a=gompertz_cache.size(); a<=age; ++a){
                double hazard = alpha*std::exp(beta*age);
                gompertz_cache.push_back(1.0-std::exp(-hazard));
            }
        }

        double prob_die = gompertz_cache[age];
        if (std::generate_canonical<double, 10>(rng) < prob_die) {
            state = AgentState::Dead;
            return true;
        }
        return false;
    }
};

struct Stats{
    int finished_count = 0;
    int dead_count = 0;

    std::vector<int> ages_finished;
    std::vector<int> ages_dead;

    std::vector<int> time_finished;
    std::vector<int> time_dead;

    void on_finish(const Agent& a, int tick){
        finished_count++;
        ages_finished.push_back(a.age);
        time_finished.push_back(tick);
    };
    
    void on_death(const Agent& a, int tick){
        dead_count++;
        ages_dead.push_back(a.age);
        time_dead.push_back(tick);
    };

    double mean(std::vector<int>& data){
        double sum = std::accumulate(std::begin(data), std::end(data), 0.0);
        double mean = sum/std::size(data);
        return mean;
    };

    int moda(std::vector<int>& data){
        std::unordered_map<int, int>contagem;

        for(int num:data){contagem[num]++;}

        int moda = -1;
        int max_freq=0;
        for (auto const& [num, freq]:contagem){
            if(freq>max_freq){
                max_freq=freq;
                moda=num;
            }
        }
        return moda;
    };
};

struct World {
    int pos_f;
    int tick;

    std::vector<int> active_ids;
    std::vector<Agent> agents;
    Stats stats;

    void step(std::mt19937& rng) {
        int next_check=100;
        for(size_t i=0; i<active_ids.size(); ){
            Agent& agent = agents[active_ids[i]];

            double p_forward = std::min(0.5 + 0.05 * agent.pos, 0.9);
            agent.move(p_forward, rng);

            if(next_check==100 || tick>=next_check){
                agent.age_up(tick);
                bool died = agent.try_die(rng, tick);
                if(died){stats.on_death(agent, tick);}
            }
            bool finished = false;

            if(agent.pos>=pos_f){
                agent.state = Agent::AgentState::Finished;
                finished = true;
            }

            if(finished){stats.on_finish(agent, tick);}
            
            if(!agent.active()){
                active_ids[i] = active_ids.back();
                active_ids.pop_back();
            } else {
                ++i;
            }
        }
        tick++;
    }

    bool finished() const {
        for (const auto& agent:agents) {
            if (agent.active())
                return false;
        }
        return true;
    }
};


int main() {
    int pos_f;
    int n_agents;

    std::mt19937 rng(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    std::cout<<"Posicao alvo: ";
    std::cin>>pos_f;

    if(pos_f <= 0) {
        std::cout<<"Posicao alvo invalida.\n";
        return 1;
    }

    std::cout<<"Quantidade de agentes: ";
    std::cin>>n_agents;

    if(n_agents<=0) {
        std::cout<<"Quantidade de agentes invalida.\n";
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    World world;
    world.pos_f = pos_f;
    world.tick = 0;

    for (int i = 0; i < n_agents; ++i) {
        world.agents.push_back(
            Agent{
                .id = i,
                .pos = 0,
                .age = 0,
                .state = Agent::AgentState::Alive
            }
        );
        world.active_ids.push_back(i);
    }

    while (!world.finished()) {
        world.step(rng);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seconds = end - start;

    std::cout<<"\nFinalizado em "<< world.tick <<" ticks.\n";
    std::cout<<"Total de agentes: "<<n_agents<<"\n";
    std::cout << "Tempo de execucao: " << seconds.count() << "s\n\n";
    
    std::cout<<"Quantidade de agentes com sucesso: "<<world.stats.finished_count<<" ("<<((world.stats.finished_count*100.0)/n_agents)<<"%)"<<"\n";
    std::cout<<"Media de tempo de sucesso: "<<world.stats.mean(world.stats.time_finished)<<" tick(s)"<<"\n";
    std::cout<<"Moda de tempo de sucesso: "<<world.stats.moda(world.stats.time_finished)<<" tick(s)"<<"\n";
    std::cout<<"Media de idade de sucesso: "<<world.stats.mean(world.stats.ages_finished)<<" ano(s)"<<"\n";
    std::cout<<"Moda de idade de sucesso: "<<world.stats.moda(world.stats.ages_finished)<<" ano(s)"<<"\n\n";

    std::cout<<"Quantidade de agentes mortos: "<<world.stats.dead_count<<" ("<<((world.stats.dead_count*100.0)/n_agents)<<"%)"<<"\n";
    std::cout<<"Media de tempo de morte: "<<world.stats.mean(world.stats.time_dead)<<" tick(s)"<<"\n";
    std::cout<<"Moda de tempo de morte: "<<world.stats.moda(world.stats.time_dead)<<" tick(s)"<<"\n";
    std::cout<<"Media de idade de morte: "<<world.stats.mean(world.stats.ages_dead)<<" ano(s)"<<"\n";
    std::cout<<"Moda de idade de morte: "<<world.stats.moda(world.stats.ages_dead)<<" ano(s)"<<"\n";

    return 0;
}
