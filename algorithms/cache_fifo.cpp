#ifndef CACHE_FIFO_CPP
#define CACHE_FIFO_CPP

#include <vector>
#include <utility>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class CacheFIFO : public AlgoritmoCache {
private:
    vector<pair<int, string>> cache;
    int capacidade;
    int hits;
    int misses;

public:
    CacheFIFO(int cap = 10) : capacidade(cap), hits(0), misses(0) {}
    
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
            cout << "🗑️  FIFO: Removendo texto " << cache[0].first << " (mais antigo)" << endl;
            cache.erase(cache.begin());
        }
        
        cache.push_back({id, conteudo});
        cout << "💾 FIFO: Texto " << id << " armazenado (" << cache.size() << "/" << capacidade << ")" << endl;
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
};

#endif