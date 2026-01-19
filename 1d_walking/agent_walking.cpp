#include <iostream>
#include <locale>
#include <vector>
#include <thread>
#include <random>


struct Agent{
    int id;
    int pos;
    int age;
    bool alive;
    bool finished;

    void move(double p_forward, std::mt19937& rng){
        std::bernoulli_distribution move(0.5);
        if(move(rng)){
            bool forward = (std::generate_canonical<double,10>(rng)<p_forward);
            pos+=forward? +1:-1;
        }
    }

    void age_up(int tick){
        age = tick/100;
    }

    void try_die(std::mt19937& rng){
        double prob_die = 1-std::exp(-age*0.095);
        if(std::generate_canonical<double, 10>(rng)<prob_die){
            alive = false;
        }
    }
};

struct World {
    int pos_f;
    int tick;

    Agent agent;

    void step(std::mt19937& rng){
        agent.move(std::min((0.5+0.05*agent.pos), 0.9), rng);
        tick++;
        agent.age_up(tick);
        agent.try_die(rng);
    };

    bool finished() const{
        return agent.pos>=pos_f || !agent.alive;
    };
};

int main() {
    int pos_f;

    std::mt19937 rng(std::hash<std::thread::id>{}(std::this_thread::get_id()));

    std::cout<<"Posicao alvo: ";
    std::cin>>pos_f;

    if(pos_f<=0){
        std::cout<<"Posicao alvo invalida.\n";
        return 1;
    }

    World world{
        .pos_f = pos_f,
        .tick = 0,
        .agent = {
            .pos = 0,
            .age = 0,
            .alive = true
        }
    };
    
    while(!world.finished()){
        world.step(rng);
    }

    std::cout<<"Finalizado em "<<world.tick<<" ticks.\n";
    std::cout<<"Idade do agente: "<<world.agent.age<<" anos.\n";
    std::cout<<"Status do agente: "<<world.agent.alive<<"\n";
    std::cout<<"Posicao final do agente: "<<world.agent.pos<<std::endl;
    return 0;
}