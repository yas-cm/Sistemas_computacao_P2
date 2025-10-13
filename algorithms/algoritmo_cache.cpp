#ifndef ALGORITMO_CACHE_CPP
#define ALGORITMO_CACHE_CPP

#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class AlgoritmoCache {
public:
    virtual ~AlgoritmoCache() = default;
    
    virtual string buscar_texto(int id) = 0; // Busca texto pelo identificador único

    virtual void carregar_texto(int id, const string& conteudo) = 0; // Insere ou atualiza um texto no cache

    virtual pair<int, int> get_estatisticas() const = 0; // Retorna estatísticas de hits e misses

    virtual string get_nome() const = 0; // Nome do algoritmo implementado

    virtual void limpar_cache() = 0; // Reseta o estado interno do cache

    virtual vector<int> get_ids_cache() const = 0; // Retorna os IDs atualmente armazenados

    virtual void set_modo_silencioso(bool silencioso) = 0; // Ativa ou desativa logs de saída
};

#endif