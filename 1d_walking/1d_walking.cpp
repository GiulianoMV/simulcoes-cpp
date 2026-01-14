#include <iostream>
#include <locale>
#include <random>
#include <functional>
#include <thread>
#include <future>
#include <vector>
#include <chrono>


std::pair<int,int> exec_runs(int runs, int pos_f){
    std::mt19937 rng(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::bernoulli_distribution move(0.5);

    int ok=0, nok=0;

    for(int i=0; i<runs; i++){
        int ticks=0, pos=0;
        while(pos!=pos_f){
            double p_forward=0.5+0.05*pos;
            p_forward=std::min(p_forward,0.9);

            if(move(rng)){
                bool forward=(std::generate_canonical<double,10>(rng)<p_forward);
                int velocity=forward?+1:-1;
                pos+=velocity;
            }
            ticks++;

            if(pos<0 && ticks<pos_f*0.2){
                nok++;
                break;
            }
        }
        if(pos==pos_f){ok++;}
    }
    return {ok, nok};
}

int main(){
    std::setlocale(LC_ALL, "");
    int runs, pos_f;

    std::cout<<"Quantidade de runs: ";
    std::cin>>runs;

    std::cout<<"Posição alvo: ";
    std::cin>>pos_f;

    if(pos_f<0 || runs<0){
        std::cout<<"Parametros inválidos.\n";
        return 1;
    }

    int n_threads = std::thread::hardware_concurrency();
    if(n_threads==0){n_threads=1;}
    
    char decisor;
    std::cout<<"Gostaria de forçar a execução sequencial? (Y/n) ";
    std::cin>>decisor;
    if(decisor=='Y'||decisor=='y'){n_threads=1;}
    int runs_per_thread=runs/n_threads;

    std::vector<std::future<std::pair<int,int>>> futures;

    auto start=std::chrono::high_resolution_clock::now();

    for(int i=0; i<n_threads; i++){
        futures.push_back(
            std::async(std::launch::async, exec_runs, runs_per_thread, pos_f)
        );
    }

    int ok=0, nok=0;

    for(auto& f:futures){
        auto [ok_t,nok_t]=f.get();
        ok+=ok_t;
        nok+=nok_t;
    }

    auto end=std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seconds = end - start;

    std::cout<<"\nResultados:\n";
    std::cout<<"Runs por thread criada: "<<runs_per_thread<<"\n";
    std::cout<<"Quantidade de threads criadas: "<<n_threads<<"\n";
    std::cout<<"Quantidade de runs finalizadas com sucesso: "<<ok<<"\n";
    std::cout<<"Quantidade de runs finalizadas sem sucesso: "<<nok<<"\n";
    std::cout<<"Tempo de execução: "<<seconds.count()<< "s"<<std::endl;
    return 0;
}