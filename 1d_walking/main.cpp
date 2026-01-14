#include <iostream>
#include <locale>
#include <random>


int main(){
    std::setlocale(LC_ALL, "");
    std::mt19937 rng(std::random_device{}());
    std::bernoulli_distribution move(0.5);

    int runs, qtd_ok=0, qtd_nok=0;
    int pos_f;

    std::cout<<"Quantidade de runs: ";
    std::cin>>runs;

    if(runs<1){
        std::cout<<"Quantidade de runs inválida.\n";
        return 1;
    }

    std::cout<<"Posição alvo: ";
    std::cin>>pos_f;

    if(pos_f<0){
        std::cout<<"Posição alvo inválida.\n";
        return 1;
    }

    std::cout<<"\n";

    for(int i=0; i<runs; i++){
        int ticks=0, pos=0;
        while(pos!=pos_f){
            double p_forward=0.5+0.05*pos;
            p_forward=std::min(p_forward,0.9);
            
            if(move(rng)){
                bool forward = (std::generate_canonical<double,10>(rng) < p_forward);
                int velocity = forward?+1:-1;
                pos+=velocity;
            }
            ticks++;
            // std::cout<<"\nPosicao atual: "<<pos;
            // std::cout<<"\nVelocidade atual: "<<velocity;
            // std::cout<<"\nTempo atual: "<<ticks;

            if(pos<0 && ticks<pos_f*0.2){
                // std::cout<<"[RUN "<<(i+1)<<"] "<<"Você caiu do mapa.\n";
                qtd_nok++;
                break;
            }
        }
        if(pos==pos_f){
            qtd_ok++;
            // std::cout<<"[RUN "<<(i+1)<<"] "<<"Você chegou ao destino em "<<ticks<<" ticks!\n";
        }
    }

    std::cout<<"Resultados:\n";
    std::cout<<"Quantidade de runs finalizadas com sucesso: "<<qtd_ok<<"\n";
    std::cout<<"Quantidade de runs finalizadas sem sucesso: "<<qtd_nok<<"\n";
    return 0;
}