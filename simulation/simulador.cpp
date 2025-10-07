#ifndef SIMULADOR_CPP
#define SIMULADOR_CPP

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <string>
#include <thread>
#include <fstream>
#include "../algorithms/algoritmo_cache.cpp"
#include "../algorithms/cache_fifo.cpp"
#include "../algorithms/cache_lru.cpp"
#include "../algorithms/cache_mru.cpp"

using namespace std;

// ✅ ESTRUTURA PARA ARMAZENAR RESULTADOS
struct ResultadoSimulacao {
    vector<string> algoritmos;
    vector<double> tempo_medio;
    vector<double> taxa_hit;
    vector<int> total_misses;
    vector<int> total_hits;
};

class Simulador {
private:
    vector<string> caminhos_textos;

public:
    Simulador() {
        carregar_lista_textos();
    }

    void carregar_lista_textos() {
        caminhos_textos.resize(100);
        for (int i = 0; i < 100; i++) {
            caminhos_textos[i] = "texts/" + to_string(i + 1) + ".txt";
        }
    }

    string carregar_texto_disco_simulacao(int id) {
        this_thread::sleep_for(chrono::milliseconds(100));
        return "Conteúdo simulado do texto " + to_string(id);
    }

    // ✅ RETORNA OS RESULTADOS COMPLETOS PARA OS GRÁFICOS
    ResultadoSimulacao executar_simulacao_completa() {
        cout << "\n🎮 INICIANDO MODO SIMULAÇÃO..." << endl;
        cout << "Testando 3 algoritmos com 3 usuários..." << endl;
        
        CacheFIFO algoritmo_fifo;
        CacheLRU algoritmo_lru;
        CacheMRU algoritmo_mru;
        
        vector<pair<AlgoritmoCache*, string>> algoritmos = {
            {&algoritmo_fifo, "FIFO"},
            {&algoritmo_lru, "LRU"},
            {&algoritmo_mru, "MRU"}
        };
        
        ResultadoSimulacao resultados;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist_aleatorio(1, 100);
        poisson_distribution<> dist_poisson(50);
        uniform_real_distribution<> dist_prob(0.0, 1.0);
        
        for (auto& [algoritmo, nome] : algoritmos) {
            cout << "\n🔍 Testando " << nome << "..." << endl;
            algoritmo->set_modo_silencioso(true);
            
            double tempo_total = 0;
            int total_requisicoes = 0;
            
            for (int usuario = 1; usuario <= 3; usuario++) {
                cout << "   👤 Usuário " << usuario << ": ";
                
                for (int req = 1; req <= 200; req++) {
                    int texto_id;
                    
                    if (req <= 66) {
                        texto_id = dist_aleatorio(gen);
                    } else if (req <= 132) {
                        texto_id = min(max(dist_poisson(gen), 1), 100);
                    } else {
                        if (dist_prob(gen) < 0.43) {
                            uniform_int_distribution<> dist_ponderado(30, 40);
                            texto_id = dist_ponderado(gen);
                        } else {
                            texto_id = dist_aleatorio(gen);
                        }
                    }
                    
                    auto inicio = chrono::steady_clock::now();
                    string conteudo = algoritmo->buscar_texto(texto_id);
                    if (conteudo.empty()) {
                        string conteudo_disco = carregar_texto_disco_simulacao(texto_id);
                        algoritmo->carregar_texto(texto_id, conteudo_disco);
                    }
                    auto fim = chrono::steady_clock::now();
                    auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
                    tempo_total += duracao.count();
                    total_requisicoes++;
                }
                cout << "✓" << endl;
            }
            
            // ✅ COLETAR DADOS PARA OS GRÁFICOS
            auto stats = algoritmo->get_estatisticas();
            double tempo_medio = tempo_total / total_requisicoes;
            double taxa_hit = (stats.first * 100.0) / max(1, stats.first + stats.second);
            
            resultados.algoritmos.push_back(nome);
            resultados.tempo_medio.push_back(tempo_medio);
            resultados.taxa_hit.push_back(taxa_hit);
            resultados.total_misses.push_back(stats.second);
            resultados.total_hits.push_back(stats.first);
            
            cout << "   ✅ " << nome << " - Tempo: " << tempo_medio << "ms";
            cout << " | Hits: " << stats.first << " | Misses: " << stats.second;
            cout << " | Taxa: " << taxa_hit << "%" << endl;
        }
        
        return resultados;
    }

    // ✅ SALVAR RESULTADOS EM ARQUIVO PARA O PYTHON
    void salvar_resultados_para_grafico(const ResultadoSimulacao& resultados) {
        ofstream arquivo("simulation/resultados_simulacao.txt");
        
        if (arquivo.is_open()) {
            // Salvar algoritmos
            arquivo << "ALGORITMOS:";
            for (const auto& algo : resultados.algoritmos) {
                arquivo << " " << algo;
            }
            arquivo << "\n";
            
            // Salvar tempo médio
            arquivo << "TEMPO_MEDIO:";
            for (double tempo : resultados.tempo_medio) {
                arquivo << " " << tempo;
            }
            arquivo << "\n";
            
            // Salvar taxa de hit
            arquivo << "TAXA_HIT:";
            for (double taxa : resultados.taxa_hit) {
                arquivo << " " << taxa;
            }
            arquivo << "\n";
            
            // Salvar total misses
            arquivo << "TOTAL_MISSES:";
            for (int misses : resultados.total_misses) {
                arquivo << " " << misses;
            }
            arquivo << "\n";
            
            arquivo.close();
            cout << "💾 Dados salvos em: simulation/resultados_simulacao.txt" << endl;
        }
    }

    // ✅ EXECUTAR SIMULAÇÃO E RETORNAR ALGORITMO VENCEDOR
    string executar_simulacao() {
        ResultadoSimulacao resultados = executar_simulacao_completa();
        salvar_resultados_para_grafico(resultados);
        
        // Encontrar algoritmo com melhor taxa de hit
        string algoritmo_vencedor = resultados.algoritmos[0];
        double melhor_taxa = resultados.taxa_hit[0];
        
        for (size_t i = 1; i < resultados.algoritmos.size(); i++) {
            if (resultados.taxa_hit[i] > melhor_taxa) {
                melhor_taxa = resultados.taxa_hit[i];
                algoritmo_vencedor = resultados.algoritmos[i];
            }
        }
        
        cout << "\nALGORITMO VENCEDOR: " << algoritmo_vencedor << endl;
        cout << "Gerando graficos e relatorios..." << endl;
        
        // ✅ APENAS UMA CHAMADA AO PYTHON - SEM EMOJIS
        system("python simulation/gerador_graficos.py");
        
        return algoritmo_vencedor;
    }
};

#endif