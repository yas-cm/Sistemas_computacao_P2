#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

using namespace std;

// ============================================================================
// INTERFACE PARA ALGORITMOS DE CACHE
// ============================================================================

class AlgoritmoCache {
public:
    virtual ~AlgoritmoCache() = default;
    virtual string buscar_texto(int id) = 0;
    virtual void carregar_texto(int id, const string& conteudo) = 0;
    virtual pair<int, int> get_estatisticas() const = 0;
    virtual string get_nome() const = 0;
    virtual void limpar_cache() = 0;
};

// ============================================================================
// ALUNO B - CACHE FIFO 
// ============================================================================

class CacheFIFO : public AlgoritmoCache {
private:
    vector<pair<int, string>> cache;
    int capacidade;
    int hits;
    int misses;

public:
    CacheFIFO(int cap = 10) : capacidade(cap), hits(0), misses(0) {
        cout << "🔄 Cache FIFO criado (capacidade: " << capacidade << " textos)" << endl;
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
        // Verifica se já está no cache
        for (const auto& item : cache) {
            if (item.first == id) {
                return;  // Já está, não faz nada
            }
        }
        
        // Se cache cheio, remove o primeiro (FIFO)
        if (cache.size() >= capacidade) {
            cout << "🗑️  FIFO: Removendo texto " << cache[0].first << " (mais antigo)" << endl;
            cache.erase(cache.begin());
        }
        
        // Adiciona novo texto no final
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

// ============================================================================
// ALUNO A - GERENCIADOR PRINCIPAL
// ============================================================================

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

        // 1ª BUSCA: Tenta pegar do cache
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
            
            // Carrega do disco (lento)
            string conteudo_disco = carregar_texto_disco(id);
            
            // ARMAZENA NO CACHE antes de mostrar
            if (algoritmo_cache) {
                algoritmo_cache->carregar_texto(id, conteudo_disco);
                
                // 2ª BUSCA: Pega do cache (agora está lá)
                conteudo = algoritmo_cache->buscar_texto(id);
                if (!conteudo.empty()) {
                    cout << "💾 Texto armazenado no cache e recuperado com sucesso!" << endl;
                }
            } else {
                conteudo = conteudo_disco;
            }
        }
        
        // MOSTRA TEXTO (agora sempre pode vir do cache)
        cout << "📄 Texto " << id << ":" << endl;
        cout << "==========================================" << endl;
        
        // Mostra primeiras linhas
        size_t pos = 0;
        int linhas_mostradas = 0;
        while (pos < conteudo.length() && linhas_mostradas < 3) {
            size_t newline = conteudo.find('\n', pos);
            if (newline == string::npos) break;
            cout << conteudo.substr(pos, newline - pos) << endl;
            pos = newline + 1;
            linhas_mostradas++;
        }
        if (linhas_mostradas == 3 && pos < conteudo.length()) {
            cout << "... [conteúdo truncado]" << endl;
        }
        
        cout << "==========================================" << endl;

        // MOSTRA TEMPO
        auto fim = chrono::steady_clock::now();
        auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
        cout << "⏰ Tempo total: " << duracao.count() << "ms" << endl;
        
        if (cache_hit) {
            cout << "🚀 Velocidade alta: conteúdo veio do cache!" << endl;
        } else {
            cout << "🐌 Velocidade baixa: conteúdo veio do disco forense" << endl;
        }
    }

    void executar_modo_simulacao() {
        cout << "\n🎮 MODO SIMULAÇÃO" << endl;
        cout << "================" << endl;
        cout << "Este modo testará diferentes algoritmos de cache" << endl;
        cout << "com usuários virtuais fazendo 200 solicitações cada." << endl;
        cout << "Aguardando implementação do Aluno D..." << endl;
        
        // Simulação básica para teste
        cout << "⏳ Executando simulação básica..." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        cout << "✅ Simulação concluída!" << endl;
        cout << "📊 Em breve: relatórios comparativos entre algoritmos" << endl;
    }

    void mostrar_estatisticas() {
        if (algoritmo_cache) {
            auto stats = algoritmo_cache->get_estatisticas();
            cout << "\n📊 ESTATÍSTICAS DO SISTEMA:" << endl;
            cout << "Algoritmo: " << algoritmo_cache->get_nome() << endl;
            cout << "Hits: " << hits << " | Misses: " << misses << endl;
            cout << "Taxa de acerto: " << (hits * 100.0 / max(1, hits + misses)) << "%" << endl;
            
            // Estatísticas do algoritmo específico
            auto algo_stats = algoritmo_cache->get_estatisticas();
            cout << "Estatísticas do algoritmo:" << endl;
            cout << "  Hits: " << algo_stats.first << " | Misses: " << algo_stats.second << endl;
        } else {
            cout << "❌ Nenhum algoritmo de cache configurado!" << endl;
        }
    }
};

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    cout << "📚 SISTEMA DE LEITURA - TEXTO É VIDA" << endl;
    cout << "====================================" << endl;
    cout << "Cache: 10 textos | Disco: 100 textos" << endl;
    cout << "====================================" << endl;
    
    GerenciadorTextos gerenciador;
    
    // ✅ ALUNO B - FIFO INTEGRADO
    CacheFIFO algoritmo_fifo;
    gerenciador.set_algoritmo_cache(&algoritmo_fifo);
    cout << "✅ Algoritmo: " << algoritmo_fifo.get_nome() << endl;

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
            cout << "👋 Saindo do programa..." << endl;
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
            cout << "❌ Número inválido! Use 1-100, -1 para simulação ou 0 para sair." << endl;
        }
        
    } while (true);
    
    cout << "\nObrigado por usar o sistema! 📖" << endl;
    return 0;
}