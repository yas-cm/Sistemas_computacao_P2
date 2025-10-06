#ifndef ALGORITMO_CACHE_CPP
#define ALGORITMO_CACHE_CPP

#include <iostream>
#include <string>
#include <utility>

using namespace std;

class AlgoritmoCache {
public:
    virtual ~AlgoritmoCache() = default;
    virtual string buscar_texto(int id) = 0;
    virtual void carregar_texto(int id, const string& conteudo) = 0;
    virtual pair<int, int> get_estatisticas() const = 0;
    virtual string get_nome() const = 0;
    virtual void limpar_cache() = 0;
};

#endif