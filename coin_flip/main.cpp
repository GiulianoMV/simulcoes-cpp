#include <iostream>
#include <cstdlib>
#include <ctime>
#include <locale>


int main() {
    std::setlocale(LC_ALL, "");
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int lancamentos;

    std::cout<<"Quantidade de Lançamentos: ";
    std::cin>>lancamentos;

    if(lancamentos<=0){
        std::cout<<"Quantidade inválida.\n";
        return 1;
    }

    int caras=0, coroas=0;

    for(int i=0; i<lancamentos; i++){
        int resultado=std::rand()%2;

        if(resultado==0){
            caras++;
            //std::cout<<"\nLançamento "<<(i+1)<<": Cara";
        } else {
            coroas++;
            //std::cout<<"\nLançamento "<<(i+1)<<": Coroa";
        }
    }

    std::cout<<"\nResultados:\n";
    std::cout<<"Caras: "<<caras<<" ("<<((100.0*caras)/lancamentos)<<"%)\n";
    std::cout<<"Coroas: "<<coroas<<" ("<<((100.0*coroas)/lancamentos)<<"%)\n";

    return 0;
}