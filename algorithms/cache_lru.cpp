#ifndef CACHE_LRU_CPP
#define CACHE_LRU_CPP

#include <vector>
#include <list>
#include <unordered_map>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class CacheLRU : public AlgoritmoCache {
private:
    int capacidade;
    int hits;
    int misses;
    list<pair<int, string>> cache_list;
    unordered_map<int, list<pair<int, string>>::iterator> cache_map;

public:
    CacheLRU(int cap = 10) : capacidade(cap), hits(0), misses(0) {}
    
    string buscar_texto(int id) override {
        auto it = cache_map.find(id);
        if (it != cache_map.end()) {
            cache_list.splice(cache_list.end(), cache_list, it->second);
            hits++;
            return it->second->second;
        }
        misses++;
        return "";
    }

    void carregar_texto(int id, const string& conteudo) override {
        auto it = cache_map.find(id);
        if (it != cache_map.end()) {
            it->second->second = conteudo;
            cache_list.splice(cache_list.end(), cache_list, it->second);
            return;
        }
        
        if (cache_list.size() >= capacidade) {
            int id_remover = cache_list.front().first;
            cout << "🗑️  LRU: Removendo texto " << id_remover << " (menos recentemente usado)" << endl;
            cache_map.erase(id_remover);
            cache_list.pop_front();
        }
        
        cache_list.push_back({id, conteudo});
        cache_map[id] = prev(cache_list.end());
        cout << "💾 LRU: Texto " << id << " armazenado (" << cache_list.size() << "/" << capacidade << ")" << endl;
    }

    pair<int, int> get_estatisticas() const override {
        return {hits, misses};
    }

    string get_nome() const override {
        return "LRU (Least Recently Used)";
    }

    void limpar_cache() override {
        cache_list.clear();
        cache_map.clear();
        hits = 0;
        misses = 0;
    }
};

#endif