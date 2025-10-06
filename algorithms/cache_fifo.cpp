#ifndef CACHE_FIFO_CPP
#define CACHE_FIFO_CPP

#include <vector>
#include <utility>
#include <iostream>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class CacheFIFO : public AlgoritmoCache {
private:
    vector<pair<int, string>> cache;
    int capacidade;
    int hits;
    int misses;
    bool modo_silencioso;  // ✅ NOVO: controla se mostra logs

public:
    CacheFIFO(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) {  // ✅ NOVO método
        modo_silencioso = silencioso;
    }
    
    string buscar_texto(int id) override {
        for (const auto& item : cache) {
            if (item.first == id) {
                hits++;
                return item.second;
            }
        }
        misses++;
        return "";
    }

    void carregar_texto(int id, const string& conteudo) override {
        for (const auto& item : cache) {
            if (item.first == id) return;
        }
        
        if (cache.size() >= capacidade) {
            if (!modo_silencioso) {  // ✅ Só mostra se não estiver silencioso
                cout << "🗑️  FIFO: Removendo texto " << cache[0].first << " (mais antigo)" << endl;
            }
            cache.erase(cache.begin());
        }
        
        cache.push_back({id, conteudo});
        if (!modo_silencioso) {  // ✅ Só mostra se não estiver silencioso
            cout << "💾 FIFO: Texto " << id << " armazenado (" << cache.size() << "/" << capacidade << ")" << endl;
        }
    }

    pair<int, int> get_estatisticas() const override {
        return {hits, misses};
    }

    string get_nome() const override {
        return "FIFO (First-In, First-Out)";
    }

    void limpar_cache() override {
        cache.clear();
        hits = 0;
        misses = 0;
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;
        for (const auto& item : cache) {
            ids.push_back(item.first);
        }
        return ids;
    }
};

#endif