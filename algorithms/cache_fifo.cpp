#ifndef CACHE_FIFO_CPP
#define CACHE_FIFO_CPP

#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

class CacheFIFO {
private:
    vector<pair<int, string>> cache;  // Armazena (id, conteúdo)
    int capacidade;
    int hits;
    int misses;

public:
    // Construtor
    CacheFIFO(int cap = 10) : capacidade(cap), hits(0), misses(0) {
        cout << "🔄 Cache FIFO criado (capacidade: " << capacidade << " textos)" << endl;
    }
    
    // Busca texto no cache - retorna conteúdo se encontrou, string vazia se não
    string buscar_texto(int id) {
        // Procura o texto no cache
        for (const auto& item : cache) {
            if (item.first == id) {
                hits++;
                cout << "🔍 Cache FIFO: ENCONTRADO texto " << id << " (HIT)" << endl;
                return item.second;
            }
        }
        misses++;
        cout << "🔍 Cache FIFO: NÃO ENCONTRADO texto " << id << " (MISS)" << endl;
        return "";
    }

    // Carrega texto no cache (lógica FIFO)
    void carregar_texto(int id, const string& conteudo) {
        // Verifica se já está no cache (não deve duplicar)
        for (const auto& item : cache) {
            if (item.first == id) {
                cout << "⚠️  Cache FIFO: Texto " << id << " já está no cache" << endl;
                return;
            }
        }
        
        // Se cache cheio, remove o primeiro (FIFO)
        if (cache.size() >= capacidade) {
            cout << "🗑️  Cache FIFO: Removendo texto " << cache[0].first << " (FIFO)" << endl;
            cache.erase(cache.begin());
        }
        
        // Adiciona novo texto no final
        cache.push_back({id, conteudo});
        cout << "💾 Cache FIFO: Texto " << id << " armazenado (total: " << cache.size() << "/" << capacidade << ")" << endl;
    }

    // Retorna estatísticas (hits, misses)
    pair<int, int> get_estatisticas() const {
        return {hits, misses};
    }

    // Retorna nome do algoritmo
    string get_nome() const {
        return "FIFO (First-In, First-Out)";
    }

    // Limpa o cache completamente
    void limpar_cache() {
        cache.clear();
        hits = 0;
        misses = 0;
        cout << "🧹 Cache FIFO: Limpo completamente!" << endl;
    }

    // Método auxiliar para ver o que está no cache
    void mostrar_conteudo_cache() const {
        cout << "📦 Conteúdo do Cache FIFO: ";
        if (cache.empty()) {
            cout << "VAZIO";
        } else {
            for (const auto& item : cache) {
                cout << item.first << " ";
            }
        }
        cout << " [" << cache.size() << "/" << capacidade << "]" << endl;
    }
};

#endif