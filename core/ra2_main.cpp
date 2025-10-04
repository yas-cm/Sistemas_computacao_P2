#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

using namespace std;

// APENAS a interface - implementação fica com outros alunos
class AlgoritmoCache {
public:
    virtual ~AlgoritmoCache() = default;
    virtual string buscar_texto(int id) = 0;           // Aluno B, C, D implementam
    virtual void carregar_texto(int id, const string& conteudo) = 0;  // Aluno B, C, D implementam
    virtual pair<int, int> get_estatisticas() const = 0;              // Aluno B, C, D implementam
    virtual string get_nome() const = 0;                              // Aluno B, C, D implementam
    virtual void limpar_cache() = 0;                                  // Aluno B, C, D implementam
};

// Gerenciador principal - APENAS Aluno A
class GerenciadorTextos {
private:
    vector<string> caminhos_textos;
    AlgoritmoCache* algoritmo_cache;
    int hits;
    int misses;

public:
    GerenciadorTextos() : algoritmo_cache(nullptr), hits(0), misses(0) {
        carregar_lista_textos();
    }

    void set_algoritmo_cache(AlgoritmoCache* algoritmo) {
        algoritmo_cache = algoritmo;
    }

    void carregar_lista_textos() {
        caminhos_textos.resize(100);
        for (int i = 0; i < 100; i++) {
            caminhos_textos[i] = "texts/" + to_string(i + 1) + ".txt";
        }
    }

    string carregar_texto_disco(int id) {
        // Simula tempo de carregamento lento do disco
        this_thread::sleep_for(chrono::milliseconds(100));
        
        if (id >= 1 && id <= 100) {
            string caminho = caminhos_textos[id - 1];
            ifstream arquivo(caminho);
            
            if (arquivo.is_open()) {
                string conteudo;
                string linha;
                
                while (getline(arquivo, linha)) {
                    conteudo += linha + "\n";
                }
                
                arquivo.close();
                return conteudo.empty() ? "❌ Arquivo vazio!" : conteudo;
            }
            return "❌ Arquivo não encontrado!";
        }
        return "❌ Texto não encontrado!";
    }

    void abrir_texto(int id) {
        auto inicio = chrono::steady_clock::now();
        
        if (id < 1 || id > 100) {
            cout << "❌ ID inválido! Digite um número entre 1 e 100." << endl;
            return;
        }

        string conteudo;
        bool cache_hit = false;

        // Interface com cache - implementação fica com outros alunos
        if (algoritmo_cache) {
            conteudo = algoritmo_cache->buscar_texto(id);
            if (!conteudo.empty()) {
                hits++;
                cache_hit = true;
                cout << "✅ [CACHE HIT] Texto " << id << " carregado do cache!" << endl;
            }
        }

        if (!cache_hit) {
            misses++;
            cout << "⏳ [CACHE MISS] Carregando texto " << id << " do disco..." << endl;
            conteudo = carregar_texto_disco(id);
            
            // Interface com cache - implementação fica com outros alunos
            if (algoritmo_cache) {
                algoritmo_cache->carregar_texto(id, conteudo);
            }
        }
        
        // Mostrar texto (Aluno A)
        cout << "📄 Texto " << id << " aberto:" << endl;
        cout << "==========================================" << endl;
        
        // Mostra primeiras linhas
        size_t pos = 0;
        int linhas_mostradas = 0;
        while (pos < conteudo.length() && linhas_mostradas < 5) {
            size_t newline = conteudo.find('\n', pos);
            if (newline == string::npos) break;
            cout << conteudo.substr(pos, newline - pos) << endl;
            pos = newline + 1;
            linhas_mostradas++;
        }
        if (linhas_mostradas == 5) cout << "... [conteúdo truncado]" << endl;
        
        cout << "==========================================" << endl;

        auto fim = chrono::steady_clock::now();
        auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
        cout << "⏰ Tempo total: " << duracao.count() << "ms" << endl;
    }

    void executar_modo_simulacao() {
        cout << "\n🎮 MODO SIMULAÇÃO" << endl;
        cout << "================" << endl;
        cout << "Aguardando implementação do Aluno D..." << endl;
        // Aluno D implementa
    }

    void mostrar_estatisticas() {
        if (algoritmo_cache) {
            auto stats = algoritmo_cache->get_estatisticas();
            cout << "\n📊 ESTATÍSTICAS:" << endl;
            cout << "Algoritmo: " << algoritmo_cache->get_nome() << endl;
            cout << "Hits: " << hits << " | Misses: " << misses << endl;
            cout << "Taxa de acerto: " << (hits * 100.0 / max(1, hits + misses)) << "%" << endl;
        } else {
            cout << "❌ Nenhum algoritmo de cache configurado!" << endl;
        }
    }
};

// FUNÇÃO PRINCIPAL - Aluno A
int main() {
    cout << "📚 SISTEMA DE LEITURA - TEXTO É VIDA" << endl;
    cout << "====================================" << endl;
    
    GerenciadorTextos gerenciador;
    
    // TODO: Aluno B, C, D conectam seus algoritmos aqui
    // gerenciador.set_algoritmo_cache(&algoritmo_fifo); // Aluno B
    // gerenciador.set_algoritmo_cache(&algoritmo_lru);  // Aluno C  
    // gerenciador.set_algoritmo_cache(&algoritmo_lfu);  // Aluno D

    int opcao;
    
    do {
        cout << "\nDigite o número do texto (1-100), -1 para simulação, 0 para sair: ";
        
        if (!(cin >> opcao)) {
            cout << "❌ Entrada inválida!" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        
        if (opcao == 0) {
            cout << "👋 Saindo..." << endl;
            gerenciador.mostrar_estatisticas();
            break;
        }
        else if (opcao == -1) {
            gerenciador.executar_modo_simulacao();
        }
        else if (opcao >= 1 && opcao <= 100) {
            gerenciador.abrir_texto(opcao);
        }
        else {
            cout << "❌ Número inválido!" << endl;
        }
        
    } while (true);
    
    return 0;
}