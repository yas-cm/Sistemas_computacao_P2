#ifndef CACHE_MRU_CPP
#define CACHE_MRU_CPP

#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class CacheMRU : public AlgoritmoCache {
private:
    int capacidade;
    int hits;
    int misses;
    list<pair<int, string>> cache_list;
    unordered_map<int, list<pair<int, string>>::iterator> cache_map;
    bool modo_silencioso;  // ✅ NOVO: controla se mostra logs

public:
    CacheMRU(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) {  // ✅ NOVO método
        modo_silencioso = silencioso;
    }
    
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
            int id_remover = cache_list.back().first;
            if (!modo_silencioso) {  // ✅ Só mostra se não estiver silencioso
                cout << "🗑️  MRU: Removendo texto " << id_remover << " (mais recentemente usado)" << endl;
            }
            cache_map.erase(id_remover);
            cache_list.pop_back();
        }
        
        cache_list.push_back({id, conteudo});
        cache_map[id] = prev(cache_list.end());
        if (!modo_silencioso) {  // ✅ Só mostra se não estiver silencioso
            cout << "💾 MRU: Texto " << id << " armazenado (" << cache_list.size() << "/" << capacidade << ")" << endl;
        }
    }

    pair<int, int> get_estatisticas() const override {
        return {hits, misses};
    }

    string get_nome() const override {
        return "MRU (Most Recently Used)";
    }

    void limpar_cache() override {
        cache_list.clear();
        cache_map.clear();
        hits = 0;
        misses = 0;
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;
        for (const auto& item : cache_list) {
            ids.push_back(item.first);
        }
        return ids;
    }
};

#endif