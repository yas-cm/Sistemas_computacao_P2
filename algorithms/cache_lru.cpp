#ifndef CACHE_LRU_CPP
#define CACHE_LRU_CPP

#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include "../algorithms/algoritmo_cache.cpp"

using namespace std;

class CacheLRU : public AlgoritmoCache {
private:
    int capacidade; // Tamanho máximo do cache
    int hits; // Contador de acertos
    int misses; // Contador de falhas
    list<pair<int, string>> cache_list; // Lista para manter a ordem de uso
    unordered_map<int, list<pair<int, string>>::iterator> cache_map; // Mapeia IDs para posições na lista
    bool modo_silencioso; // Controla logs de saída

public:
    CacheLRU(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) {
        modo_silencioso = silencioso;
    }
    
    string buscar_texto(int id) override {
        auto it = cache_map.find(id);
        if (it != cache_map.end()) {
            cache_list.splice(cache_list.end(), cache_list, it->second); // Move para o final (mais recente)
            hits++;
            return it->second->second; // Retorna o conteúdo
        }
        misses++;
        return ""; // Retorna vazio se não encontrado
    }

    void carregar_texto(int id, const string& conteudo) override {
        auto it = cache_map.find(id);
        if (it != cache_map.end()) {
            it->second->second = conteudo; // Atualiza o conteúdo existente
            cache_list.splice(cache_list.end(), cache_list, it->second);
            return;
        }
        
        if (cache_list.size() >= capacidade) {
            int id_remover = cache_list.front().first; // Remove o menos recentemente usado
            if (!modo_silencioso) {
                cout << "🗑️  LRU: Removendo texto " << id_remover << endl;
            }
            cache_map.erase(id_remover);
            cache_list.pop_front();
        }
        
        cache_list.push_back({id, conteudo}); // Adiciona o novo texto
        cache_map[id] = prev(cache_list.end());
        if (!modo_silencioso) {
            cout << "💾 LRU: Texto " << id << " armazenado" << endl;
        }
    }

    pair<int, int> get_estatisticas() const override {
        return {hits, misses}; // Retorna hits e misses
    }

    string get_nome() const override {
        return "LRU (Least Recently Used)"; // Nome do algoritmo
    }

    void limpar_cache() override {
        cache_list.clear(); // Limpa a lista
        cache_map.clear(); // Limpa o mapa
        hits = 0;
        misses = 0; // Reseta as estatísticas
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;
        for (const auto& item : cache_list) {
            ids.push_back(item.first); // Adiciona os IDs da lista
        }
        return ids;
    }
};

#endif