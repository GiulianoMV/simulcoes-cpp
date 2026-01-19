#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <map>
#include <algorithm>
#include <chrono>


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
        
        double alpha = 0.0001;
        double beta = 0.08;

        double hazard = alpha * std::exp(beta * age);
        double prob_die = 1.0 - std::exp(-hazard);

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

    double mean(std::vector<int> data){
        double sum = std::accumulate(std::begin(data), std::end(data), 0.0);
        double mean = sum/std::size(data);
        return mean;
    };

    int moda(std::vector<int> data){
        std::map<int,int>contagem;
        
        for(int num:data){
            contagem[num]++;
        }

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

    std::vector<Agent> agents;
    Stats stats;

    void step(std::mt19937& rng) {
        for (auto& agent:agents) {
            if (!agent.active())
                continue;

            double p_forward = std::min(0.5 + 0.05 * agent.pos, 0.9);
            agent.move(p_forward, rng);

            agent.age_up(tick);
            if(agent.try_die(rng, tick)){
                stats.on_death(agent, tick);
            }

            if(agent.pos >= pos_f) {
                agent.state = Agent::AgentState::Finished;
                stats.on_finish(agent, tick);
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
