#include <iostream>
#include <thread>

int main(){
    std::cout<<"Número de threads: "<<std::thread::hardware_concurrency()<<std::endl;
}