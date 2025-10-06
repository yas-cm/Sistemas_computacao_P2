#ifndef SIMULADOR_CPP
#define SIMULADOR_CPP

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <string>
#include <thread>
#include "../algorithms/algoritmo_cache.cpp"
#include "../algorithms/cache_fifo.cpp"
#include "../algorithms/cache_lru.cpp"
#include "../algorithms/cache_mru.cpp"

using namespace std;

class Simulador {
public:
    Simulador() {}

    // Versão simplificada para simulação (sem mostrar texto)
    string carregar_texto_disco_simulacao(int id) {
        this_thread::sleep_for(chrono::milliseconds(100)); // Simula disco lento
        return "Conteúdo simulado do texto " + to_string(id);
    }

    // Executa simulação completa e retorna o nome do algoritmo vencedor
    string executar_simulacao() {
        cout << "\n🎮 INICIANDO MODO SIMULAÇÃO..." << endl;
        cout << "Testando 3 algoritmos com 3 usuários (200 requisições cada)..." << endl;
        cout << "Padrões: Aleatório, Poisson, Ponderado (30-40)" << endl;
        
        // Criar os 3 algoritmos para teste
        CacheFIFO algoritmo_fifo;
        CacheLRU algoritmo_lru;
        CacheMRU algoritmo_mru;
        
        // ✅ ATIVAR MODO SILENCIOSO para não mostrar operações
        algoritmo_fifo.set_modo_silencioso(true);
        algoritmo_lru.set_modo_silencioso(true);
        algoritmo_mru.set_modo_silencioso(true);
        
        // Vetor de algoritmos para teste
        vector<pair<AlgoritmoCache*, string>> algoritmos = {
            {&algoritmo_fifo, "FIFO"},
            {&algoritmo_lru, "LRU"},
            {&algoritmo_mru, "MRU"}
        };
        
        // Resultados da simulação
        vector<pair<string, double>> resultados;
        
        // Gerador de números aleatórios
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist_aleatorio(1, 100);
        poisson_distribution<> dist_poisson(50);
        uniform_real_distribution<> dist_prob(0.0, 1.0);
        
        for (auto& [algoritmo, nome] : algoritmos) {
            cout << "\n🔍 Testando " << nome << "..." << endl;
            
            double tempo_total = 0;
            int total_requisicoes = 0;
            
            // Simular 3 usuários
            for (int usuario = 1; usuario <= 3; usuario++) {
                // 200 requisições por usuário
                for (int req = 1; req <= 200; req++) {
                    int texto_id;
                    
                    // Alternar entre padrões de acesso
                    if (req <= 66) {
                        // Aleatório puro
                        texto_id = dist_aleatorio(gen);
                    } else if (req <= 132) {
                        // Poisson (ajustado para 1-100)
                        texto_id = min(max(dist_poisson(gen), 1), 100);
                    } else {
                        // Ponderado: 43% chance textos 30-40
                        if (dist_prob(gen) < 0.43) {
                            uniform_int_distribution<> dist_ponderado(30, 40);
                            texto_id = dist_ponderado(gen);
                        } else {
                            texto_id = dist_aleatorio(gen);
                        }
                    }
                    
                    // Medir tempo de acesso
                    auto inicio = chrono::steady_clock::now();
                    
                    // Simular acesso
                    string conteudo = algoritmo->buscar_texto(texto_id);
                    if (conteudo.empty()) {
                        // Cache miss - carregar do disco
                        string conteudo_disco = carregar_texto_disco_simulacao(texto_id);
                        algoritmo->carregar_texto(texto_id, conteudo_disco);
                    }
                    
                    auto fim = chrono::steady_clock::now();
                    auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
                    tempo_total += duracao.count();
                    total_requisicoes++;
                }
            }
            
            double tempo_medio = tempo_total / total_requisicoes;
            resultados.push_back({nome, tempo_medio});
            
            auto stats = algoritmo->get_estatisticas();
            cout << "   ✅ " << nome << " - Tempo médio: " << tempo_medio << "ms";
            cout << " | Hits: " << stats.first << " | Misses: " << stats.second;
            cout << " | Taxa acerto: " << (stats.first * 100.0 / max(1, stats.first + stats.second)) << "%" << endl;
        }
        
        // ✅ ENCONTRAR ALGORITMO MAIS RÁPIDO
        string algoritmo_vencedor = "FIFO";
        double menor_tempo = resultados[0].second;
        
        for (const auto& [nome, tempo] : resultados) {
            if (tempo < menor_tempo) {
                menor_tempo = tempo;
                algoritmo_vencedor = nome;
            }
        }
        
        // ✅ MOSTRAR RESULTADOS FINAIS
        cout << "\n🏆 RESULTADOS DA SIMULAÇÃO:" << endl;
        cout << "==========================" << endl;
        for (const auto& [nome, tempo] : resultados) {
            cout << "   " << nome << ": " << tempo << "ms";
            if (nome == algoritmo_vencedor) cout << " 🏅 (VENCEDOR)";
            cout << endl;
        }
        
        return algoritmo_vencedor;
    }
};

#endif