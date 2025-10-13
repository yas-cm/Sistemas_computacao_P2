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
    int capacidade; // Tamanho máximo do cache
    int hits; // Contador de acertos
    int misses; // Contador de falhas
    bool modo_silencioso; // Controla logs de saída
    list<int> fifo_queue; // Fila FIFO para gerenciar a ordem de inserção
    unordered_map<int, string> cache_data; // Armazena os dados do cache

public:
    CacheFIFO(int cap = 10) : capacidade(cap), hits(0), misses(0), modo_silencioso(false) {}
    
    void set_modo_silencioso(bool silencioso) override {
        modo_silencioso = silencioso;
    }
    
    string buscar_texto(int id) override {
        auto it = cache_data.find(id); // Verifica se o texto está no cache
        if (it != cache_data.end()) {
            hits++;
            return it->second; // Retorna o conteúdo se encontrado
        }
        misses++;
        return ""; // Retorna vazio se não encontrado
    }

    void carregar_texto(int id, const string& conteudo) override {
        if (cache_data.find(id) != cache_data.end()) {
            cache_data[id] = conteudo; // Atualiza o conteúdo existente
            return;
        }
        
        if (fifo_queue.size() >= capacidade) {
            int id_remover = fifo_queue.front(); // Remove o mais antigo
            fifo_queue.pop_front();
            cache_data.erase(id_remover);
            
            if (!modo_silencioso) {
                cout << "🗑️  FIFO: Removendo texto " << id_remover << endl;
            }
        }
        
        fifo_queue.push_back(id); // Adiciona o novo texto
        cache_data[id] = conteudo;
        
        if (!modo_silencioso) {
            cout << "💾 FIFO: Texto " << id << " armazenado" << endl;
        }
    }

    pair<int, int> get_estatisticas() const override {
        return {hits, misses}; // Retorna hits e misses
    }

    string get_nome() const override {
        return "FIFO (First-In, First-Out)"; // Nome do algoritmo
    }

    void limpar_cache() override {
        fifo_queue.clear(); // Limpa a fila FIFO
        cache_data.clear(); // Limpa os dados do cache
        hits = 0;
        misses = 0; // Reseta as estatísticas
    }

    vector<int> get_ids_cache() const override {
        vector<int> ids;
        for (int id : fifo_queue) {
            ids.push_back(id); // Adiciona os IDs da FIFO
        }
        return ids;
    }
};

#endif