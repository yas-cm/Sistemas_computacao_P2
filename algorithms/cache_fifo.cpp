#ifndef CACHE_FIFO_CPP
#define CACHE_FIFO_CPP

#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class CacheFIFO : public AlgoritmoCache {
private:
    int capacidade;
    int hits;
    int misses;
    bool modo_silencioso;
    // ✅ ESTRUTURA OTIMIZADA: unordered_map + queue
    list<int> fifo_queue;
    unordered_map<int, string> cache_data;

public:
    CacheFIFO(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) override {
        modo_silencioso = silencioso;
    }
    
    string buscar_texto(int id) override {
        auto it = cache_data.find(id); // ✅ BUSCA O(1)
        if (it != cache_data.end()) {
            hits++;
            return it->second;
        }
        misses++;
        return "";
    }

    void carregar_texto(int id, const string& conteudo) override {
        // Se já está no cache, apenas atualizar conteúdo
        if (cache_data.find(id) != cache_data.end()) {
            cache_data[id] = conteudo;
            return;
        }
        
        // Se cache cheio, remover o mais antigo
        if (fifo_queue.size() >= capacidade) {
            int id_remover = fifo_queue.front();
            fifo_queue.pop_front();
            cache_data.erase(id_remover);
            
            if (!modo_silencioso) {
                cout << "🗑️  FIFO: Removendo texto " << id_remover << " (mais antigo)" << endl;
            }
        }
        
        // Adicionar novo texto
        fifo_queue.push_back(id);
        cache_data[id] = conteudo;
        
        if (!modo_silencioso) {
            cout << "💾 FIFO: Texto " << id << " armazenado (" << fifo_queue.size() << "/" << capacidade << ")" << endl;
        }
    }

    pair<int, int> get_estatisticas() const override {
        return {hits, misses};
    }

    string get_nome() const override {
        return "FIFO (First-In, First-Out)";
    }

    void limpar_cache() override {
        fifo_queue.clear();
        cache_data.clear();
        hits = 0;
        misses = 0;
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;
        for (int id : fifo_queue) {
            ids.push_back(id);
        }
        return ids;
    }
};

#endif