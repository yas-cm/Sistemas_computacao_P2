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
    int capacidade;
    int hits;
    int misses;
    bool modo_silencioso;
    
    // ✅ ESTRUTURA OTIMIZADA: unordered_map + 2 queues
    list<int> fifo_queue;          // Itens novos (1º acesso)
    list<int> lru_queue;           // Itens frequentes (2+ acessos)
    unordered_map<int, string> cache_data;  // Dados dos textos
    
    // Mapas para acesso O(1) às posições nas listas
    unordered_map<int, list<int>::iterator> fifo_positions;
    unordered_map<int, list<int>::iterator> lru_positions;

public:
    Cache2Q(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) override {
        modo_silencioso = silencioso;
    }
    
    string buscar_texto(int id) override {
        auto it = cache_data.find(id);
        if (it != cache_data.end()) {
            // ✅ ACESSO O(1) - Verifica em qual fila está
            if (fifo_positions.find(id) != fifo_positions.end()) {
                // Está na FIFO - PROMOVER para LRU
                fifo_queue.erase(fifo_positions[id]);
                fifo_positions.erase(id);
                
                lru_queue.push_back(id);
                lru_positions[id] = prev(lru_queue.end());
                
                if (!modo_silencioso) {
                    cout << "🔄 2Q: Texto " << id << " promovido FIFO → LRU" << endl;
                }
            } else if (lru_positions.find(id) != lru_positions.end()) {
                // Já está na LRU - mover para o final (mais recente)
                lru_queue.erase(lru_positions[id]);
                lru_queue.push_back(id);
                lru_positions[id] = prev(lru_queue.end());
            }
            
            hits++;
            return it->second;
        }
        misses++;
        return "";
    }

    void carregar_texto(int id, const string& conteudo) override {
        // Se já está no cache, apenas atualizar
        if (cache_data.find(id) != cache_data.end()) {
            cache_data[id] = conteudo;
            return;
        }
        
        // Verificar se precisa remover algo
        if (cache_data.size() >= capacidade) {
            remover_pagina();
        }
        
        // Nova página vai para FIFO
        fifo_queue.push_back(id);
        fifo_positions[id] = prev(fifo_queue.end());
        cache_data[id] = conteudo;
        
        if (!modo_silencioso) {
            cout << "💾 2Q: Texto " << id << " adicionado à FIFO (" 
                 << fifo_queue.size() << " fifo, " << lru_queue.size() << " lru)" << endl;
        }
    }

private:
    void remover_pagina() {
        // Prioridade 1: Remover da FIFO se não estiver vazia
        if (!fifo_queue.empty()) {
            int id_remover = fifo_queue.front();
            fifo_queue.pop_front();
            fifo_positions.erase(id_remover);
            cache_data.erase(id_remover);
            
            if (!modo_silencioso) {
                cout << "🗑️  2Q: Removendo texto " << id_remover << " da FIFO" << endl;
            }
        } 
        // Prioridade 2: Remover da LRU (menos recentemente usado)
        else if (!lru_queue.empty()) {
            int id_remover = lru_queue.front();
            lru_queue.pop_front();
            lru_positions.erase(id_remover);
            cache_data.erase(id_remover);
            
            if (!modo_silencioso) {
                cout << "🗑️  2Q: Removendo texto " << id_remover << " da LRU (menos recente)" << endl;
            }
        }
    }

public:
    pair<int, int> get_estatisticas() const override {
        return {hits, misses};
    }

    string get_nome() const override {
        return "2Q (Two Queues)";
    }

    void limpar_cache() override {
        fifo_queue.clear();
        lru_queue.clear();
        fifo_positions.clear();
        lru_positions.clear();
        cache_data.clear();
        hits = 0;
        misses = 0;
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;
        
        // Adicionar IDs da FIFO (mais antigos primeiro)
        for (int id : fifo_queue) {
            ids.push_back(id);
        }
        
        // Adicionar IDs da LRU (mais antigos primeiro)
        for (int id : lru_queue) {
            ids.push_back(id);
        }
        
        return ids;
    }
};

#endif