#include <iostream>
#include <locale>
#include <random>
#include <functional>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <cmath>

struct Simulate_Stats {
    int ok = 0;
    int nok = 0;
    int t_ok = 0;
    int t_nok = 0;
};

Simulate_Stats& operator+=(Simulate_Stats& lhs, const Simulate_Stats& rhs) {
    lhs.ok += rhs.ok;
    lhs.nok += rhs.nok;
    lhs.t_ok += rhs.t_ok;
    lhs.t_nok += rhs.t_nok;
    return lhs;
}

Simulate_Stats exec_runs(int agents, int pos_f) {
    std::mt19937 rng(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    std::bernoulli_distribution move(0.5);

    Simulate_Stats stats;

    for(int i = 0; i < agents; i++) {
        int ticks = 0, pos = 0;
        bool finished = false;

        while(!finished) {
            double prob_forward = 0.5 + 0.05 * pos;
            prob_forward = std::min(prob_forward, 0.9);

            if(move(rng)) {
                bool forward = (std::generate_canonical<double, 10>(rng) < prob_forward);
                int velocity = forward ? +1 : -1;
                pos += velocity;
            }
            ticks++;

            if(pos == pos_f) {
                stats.ok++;
                stats.t_ok += ticks;
                finished = true;
                continue;
            }

            if(pos < 0) {
                double prob_die = 1 - exp(-abs(pos*0.5) * ticks);
                bool gonna_die = (std::generate_canonical<double, 10>(rng) < prob_die);
                if(gonna_die) {
                    stats.nok++;
                    stats.t_nok += ticks;
                    finished = true;
                }
            }
        }
    }
    return stats;
}

int main() {
    std::setlocale(LC_ALL, "");
    int agents, pos_f;

    std::cout << "Quantidade de agentes: ";
    std::cin >> agents;

    std::cout << "Posição alvo: ";
    std::cin >> pos_f;

    if(pos_f < 0 || agents < 0) {
        std::cout << "Parâmetros inválidos.\n";
        return 1;
    }

    int n_threads = std::thread::hardware_concurrency();
    if(n_threads == 0) {
        n_threads = 1;
    } else {
        char decisor;
        std::cout << "Gostaria de forçar a execução sequencial? (y/N) ";
        std::cin >> decisor;
        if(decisor == 'Y' || decisor == 'y') {n_threads = 1;}
    }

    int runs_per_thread = agents / n_threads;
    int runs_extra = agents % n_threads;

    std::vector<std::future<Simulate_Stats>> futures;
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < n_threads; i++) {
        int thread_runs = runs_per_thread + (i < runs_extra ? 1 : 0);
        if(thread_runs > 0) {
            futures.push_back(
                std::async(std::launch::async, exec_runs, thread_runs, pos_f)
            );
        }
    }

    Simulate_Stats total_stats;

    for(auto& f : futures) {
        Simulate_Stats thread_stats = f.get();
        total_stats += thread_stats;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seconds = end - start;

    std::cout << "\nResultados:\n";
    std::cout << "Total de agentes: " << agents << "\n";
    std::cout << "Agentes por thread criada: " << runs_per_thread << "\n";
    std::cout << "Quantidade de threads criadas: " << n_threads << "\n";
    std::cout << "Quantidade de agentes com sucesso: " << total_stats.ok << " (" 
              << (total_stats.ok * 100.0) / agents << "%)\n";
    std::cout << "Quantidade de agentes sem sucesso: " << total_stats.nok << " (" 
              << (total_stats.nok * 100.0) / agents << "%)\n";

    double avg_t_ok = total_stats.ok > 0 ? 
        static_cast<double>(total_stats.t_ok) / total_stats.ok : 0;
    double avg_t_nok = total_stats.nok > 0 ? 
        static_cast<double>(total_stats.t_nok) / total_stats.nok : 0;
    
    std::cout << "Tempo médio dos agentes com sucesso: " << avg_t_ok << " ticks\n";
    std::cout << "Tempo médio dos agentes sem sucesso: " << avg_t_nok << " ticks\n";
    std::cout << "Tempo de execução: " << seconds.count() << "s\n";
    
    return 0;
}