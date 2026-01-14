#include <iostream>
#include <cstdlib>
#include <ctime>
#include <locale>


int main() {
    std::setlocale(LC_ALL, "");
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int lancamentos, runs;
    double prob_lancamento;
    const int BINS=21;
    int hist[BINS]={0};
    int bin;
    double pct;

    std::cout<<"Quantidade de runs: ";
    std::cin>>runs;

    if(runs<=0){
        std::cout<<"Quantidade inválida.\n";
        return 1;
    }

    std::cout<<"Quantidade de Lançamentos por run: ";
    std::cin>>lancamentos;

    if(lancamentos<=0){
        std::cout<<"Quantidade inválida.\n";
        return 1;
    }

    std::cout<<"Probabilidade de cara (0.0 a 1.0): ";
    std::cin>>prob_lancamento;

    if(prob_lancamento<0.0 || prob_lancamento>1.0){
        std::cout<<"Probabilidade inválida.\n";
        return 1;
    }

    int caras_totais=0, caras=0, coroas_totais=0;
    int scale = (runs/50)+1;

    for(int i=0; i<runs; i++){
        caras=0;
        for(int j=0; j<lancamentos; j++){
                double resultado=std::rand()/(RAND_MAX+1.0);

                if(prob_lancamento>resultado){
                    caras_totais++;
                    caras++;
                }else{
                    coroas_totais++;
                }
            }
        pct=(100.0*caras)/lancamentos;
        bin=static_cast<int>(pct)-40;
        if(bin>=0 && bin<BINS){
            hist[bin]++;}
    }

    std::cout<<"\nResultados:\n";
    std::cout<<"Caras: "<<caras_totais<<" ("<<(100.0*caras_totais)/(lancamentos*runs)<<"%)\n";
    std::cout<<"Coroas: "<<coroas_totais<<" ("<<((100.0*coroas_totais)/(lancamentos*runs))<<"%)\n";
    for(int i=0; i<BINS; i++){
        std::cout<<(40+i)<<"-"<<(41+i)<<"% | ";
        for(int j=0; j<hist[i]/scale; j++){
            std::cout<<"#";
        }
        std::cout<<"\n";
    }

    return 0;
}