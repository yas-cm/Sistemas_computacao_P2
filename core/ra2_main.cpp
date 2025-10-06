#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

// ✅ INCLUIR APENAS A INTERFACE UMA VEZ
#include "../algorithms/algoritmo_cache.cpp"
// ✅ INCLUIR OS ALGORITMOS
#include "../algorithms/cache_fifo.cpp"
#include "../algorithms/cache_lru.cpp"

using namespace std;

// GERENCIADOR PRINCIPAL (mesmo código)
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
            cout << "❌ ID inválido!" << endl;
            return;
        }

        string conteudo;
        bool cache_hit = false;

        if (algoritmo_cache) {
            conteudo = algoritmo_cache->buscar_texto(id);
            if (!conteudo.empty()) {
                hits++;
                cache_hit = true;
                cout << "✅ [CACHE HIT] Texto " << id << " do cache!" << endl;
            }
        }

        if (!cache_hit) {
            misses++;
            cout << "⏳ [CACHE MISS] Carregando texto " << id << " do disco..." << endl;
            string conteudo_disco = carregar_texto_disco(id);
            
            if (algoritmo_cache) {
                algoritmo_cache->carregar_texto(id, conteudo_disco);
                conteudo = algoritmo_cache->buscar_texto(id);
            } else {
                conteudo = conteudo_disco;
            }
        }
        
        cout << "📄 Texto " << id << ":" << endl;
        cout << "==========================================" << endl;
        
        size_t pos = 0;
        int linhas_mostradas = 0;
        while (pos < conteudo.length() && linhas_mostradas < 3) {
            size_t newline = conteudo.find('\n', pos);
            if (newline == string::npos) break;
            cout << conteudo.substr(pos, newline - pos) << endl;
            pos = newline + 1;
            linhas_mostradas++;
        }
        if (linhas_mostradas == 3) cout << "... [truncado]" << endl;
        
        cout << "==========================================" << endl;

        auto fim = chrono::steady_clock::now();
        auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio);
        cout << "⏰ Tempo: " << duracao.count() << "ms" << endl;
        
        if (cache_hit) cout << "🚀 Velocidade alta: cache!" << endl;
        else cout << "🐌 Velocidade baixa: disco" << endl;
    }

    void executar_modo_simulacao() {
        cout << "\n🎮 MODO SIMULAÇÃO" << endl;
        cout << "Aguardando Aluno D..." << endl;
    }

    void mostrar_estatisticas() {
        if (algoritmo_cache) {
            auto stats = algoritmo_cache->get_estatisticas();
            cout << "\n📊 ESTATÍSTICAS:" << endl;
            cout << "Algoritmo: " << algoritmo_cache->get_nome() << endl;
            cout << "Hits: " << hits << " | Misses: " << misses << endl;
            cout << "Taxa de acerto: " << (hits * 100.0 / max(1, hits + misses)) << "%" << endl;
        }
    }
};

// FUNÇÃO PRINCIPAL
int main() {
    cout << "📚 SISTEMA DE LEITURA - TEXTO É VIDA" << endl;
    cout << "====================================" << endl;
    
    GerenciadorTextos gerenciador;
    
    cout << "\n🎯 Escolha o algoritmo:" << endl;
    cout << "1 - FIFO (First-In, First-Out)" << endl;
    cout << "2 - LRU (Least Recently Used)" << endl;
    cout << "Digite 1 ou 2: ";
    
    int escolha;
    cin >> escolha;
    
    if (escolha == 1) {
        CacheFIFO* algoritmo = new CacheFIFO();
        gerenciador.set_algoritmo_cache(algoritmo);
        cout << "✅ " << algoritmo->get_nome() << " carregado!" << endl;
    }
    else if (escolha == 2) {
        CacheLRU* algoritmo = new CacheLRU();
        gerenciador.set_algoritmo_cache(algoritmo);
        cout << "✅ " << algoritmo->get_nome() << " carregado!" << endl;
    }
    else {
        cout << "❌ Escolha inválida! Usando FIFO." << endl;
        CacheFIFO* algoritmo = new CacheFIFO();
        gerenciador.set_algoritmo_cache(algoritmo);
    }

    int opcao;
    do {
        cout << "\nDigite texto (1-100), -1 simulação, 0 sair: ";
        cin >> opcao;
        
        if (opcao == 0) {
            gerenciador.mostrar_estatisticas();
            break;
        }
        else if (opcao == -1) gerenciador.executar_modo_simulacao();
        else if (opcao >= 1 && opcao <= 100) gerenciador.abrir_texto(opcao);
        else cout << "❌ Número inválido!" << endl;
        
    } while (true);
    
    return 0;
}