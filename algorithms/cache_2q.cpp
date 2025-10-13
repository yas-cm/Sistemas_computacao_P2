#ifndef CACHE_2Q_CPP
#define CACHE_2Q_CPP

#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class Cache2Q : public AlgoritmoCache {
private:
    int capacidade; // Capacidade máxima do cache
    int hits; // Contador de acertos
    int misses; // Contador de falhas
    bool modo_silencioso; // Controla se mensagens de log são exibidas
    
    list<int> fifo_queue; // Fila FIFO para itens recém-adicionados
    list<int> lru_queue; // Fila LRU para itens acessados mais de uma vez
    unordered_map<int, string> cache_data; // Armazena os dados do cache
    
    unordered_map<int, list<int>::iterator> fifo_positions; // Mapeia posições na FIFO
    unordered_map<int, list<int>::iterator> lru_positions; // Mapeia posições na LRU

public:
    Cache2Q(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) override {
        modo_silencioso = silencioso; // Ativa/desativa logs
    }
    
    string buscar_texto(int id) override {
        auto it = cache_data.find(id);
        if (it != cache_data.end()) {
            if (fifo_positions.find(id) != fifo_positions.end()) {
                fifo_queue.erase(fifo_positions[id]); // Remove da FIFO
                fifo_positions.erase(id);

                lru_queue.push_back(id); // Promove para LRU
                lru_positions[id] = prev(lru_queue.end());

                if (!modo_silencioso) {
                    cout << "🔄 2Q: Texto " << id << " promovido FIFO → LRU" << endl;
                }
            } else if (lru_positions.find(id) != lru_positions.end()) {
                lru_queue.erase(lru_positions[id]); // Atualiza posição na LRU
                lru_queue.push_back(id);
                lru_positions[id] = prev(lru_queue.end());
            }
            
            hits++; // Incrementa acertos
            return it->second;
        }
        misses++; // Incrementa falhas
        return "";
    }

    void carregar_texto(int id, const string& conteudo) override {
        if (cache_data.find(id) != cache_data.end()) {
            cache_data[id] = conteudo; // Atualiza conteúdo existente
            return;
        }

        if (cache_data.size() >= capacidade) {
            remover_pagina(); // Remove página se cache cheio
        }

        fifo_queue.push_back(id); // Adiciona à FIFO
        fifo_positions[id] = prev(fifo_queue.end());
        cache_data[id] = conteudo;
        
        if (!modo_silencioso) {
            cout << "💾 2Q: Texto " << id << " adicionado à FIFO (" 
                 << fifo_queue.size() << " fifo, " << lru_queue.size() << " lru)" << endl;
        }
    }

private:
    void remover_pagina() {
        if (!fifo_queue.empty()) {
            int id_remover = fifo_queue.front();
            fifo_queue.pop_front(); // Remove da FIFO
            fifo_positions.erase(id_remover);
            cache_data.erase(id_remover);
            
            if (!modo_silencioso) {
                cout << "🗑️  2Q: Removendo texto " << id_remover << " da FIFO" << endl;
            }
        } 
        else if (!lru_queue.empty()) {
            int id_remover = lru_queue.front();
            lru_queue.pop_front(); // Remove da LRU
            lru_positions.erase(id_remover);
            cache_data.erase(id_remover);
            
            if (!modo_silencioso) {
                cout << "🗑️  2Q: Removendo texto " << id_remover << " da LRU (menos recente)" << endl;
            }
        }
    }

public:
    pair<int, int> get_estatisticas() const override {
        return {hits, misses}; // Retorna estatísticas de uso
    }

    string get_nome() const override {
        return "2Q (Two Queues)"; // Nome do algoritmo
    }

    void limpar_cache() override {
        fifo_queue.clear(); // Limpa FIFO
        lru_queue.clear(); // Limpa LRU
        fifo_positions.clear();
        lru_positions.clear();
        cache_data.clear();
        hits = 0;
        misses = 0; // Reseta estatísticas
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;

        for (int id : fifo_queue) {
            ids.push_back(id); // Adiciona IDs da FIFO
        }
        
        for (int id : lru_queue) {
            ids.push_back(id); // Adiciona IDs da LRU
        }
        
        return ids;
    }
};

#endif