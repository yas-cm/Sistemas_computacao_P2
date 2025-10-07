#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

// ✅ INCLUIR OS ALGORITMOS
#include "../algorithms/algoritmo_cache.cpp"
#include "../algorithms/cache_fifo.cpp"
#include "../algorithms/cache_lru.cpp"
#include "../algorithms/cache_mru.cpp"

// ✅ INCLUIR SIMULADOR
#include "../simulation/simulador.cpp"

using namespace std;

// GERENCIADOR PRINCIPAL
class GerenciadorTextos {
private:
    vector<string> caminhos_textos;
    AlgoritmoCache* algoritmo_cache;
    int hits;
    int misses;
    string algoritmo_atual;

public:
    GerenciadorTextos() : hits(0), misses(0), algoritmo_atual("FIFO") {
        carregar_lista_textos();
        // ✅ INICIA SEMPRE COM FIFO (default)
        algoritmo_cache = new CacheFIFO();
    }

    ~GerenciadorTextos() {
        delete algoritmo_cache;
    }

    void set_algoritmo_cache(AlgoritmoCache* algoritmo, const string& nome) {
        delete algoritmo_cache;
        algoritmo_cache = algoritmo;
        algoritmo_atual = nome;
        hits = 0;
        misses = 0;
        cout << "🔄 Algoritmo trocado para: " << nome << endl;
    }

    void carregar_lista_textos() {
        caminhos_textos.resize(100);
        for (int i = 0; i < 100; i++) {
            caminhos_textos[i] = "texts/" + to_string(i + 1) + ".txt";
        }
    }

    string carregar_texto_disco(int id) {
        this_thread::sleep_for(chrono::milliseconds(100)); // Simula disco lento
        
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

    void mostrar_cache() {
        if (algoritmo_cache) {
            vector<int> ids_cache = algoritmo_cache->get_ids_cache();
            cout << "🔍 CACHE ATUAL: [";
            for (size_t i = 0; i < ids_cache.size(); i++) {
                cout << ids_cache[i];
                if (i < ids_cache.size() - 1) cout << ", ";
            }
            cout << "] (" << ids_cache.size() << "/10)" << endl;
        }
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
        
        // Mostra apenas as primeiras linhas
        cout << "📄 Texto " << id << " (primeiras linhas):" << endl;
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

        mostrar_cache();
    }

    // ✅ SIMULAÇÃO CHAMANDO ARQUIVO SEPARADO
    void executar_modo_simulacao() {
        Simulador simulador;
        string algoritmo_vencedor = simulador.executar_simulacao();
        
        // Trocar para algoritmo vencedor - APENAS UMA VEZ
        cout << "\nTrocando para algoritmo: " << algoritmo_vencedor << endl;
        if (algoritmo_vencedor == "FIFO") {
            set_algoritmo_cache(new CacheFIFO(), "FIFO");
        } else if (algoritmo_vencedor == "LRU") {
            set_algoritmo_cache(new CacheLRU(), "LRU");
        } else if (algoritmo_vencedor == "MRU") {
            set_algoritmo_cache(new CacheMRU(), "MRU");
        }
        
        cout << "Agora usando: " << algoritmo_atual << " (mais rapido)" << endl;
    }

    void mostrar_estatisticas() {
        if (algoritmo_cache) {
            auto stats = algoritmo_cache->get_estatisticas();
            cout << "\n📊 ESTATÍSTICAS ATUAIS:" << endl;
            cout << "Algoritmo: " << algoritmo_atual << endl;
            cout << "Hits: " << hits << " | Misses: " << misses << endl;
            cout << "Taxa de acerto: " << (hits * 100.0 / max(1, hits + misses)) << "%" << endl;
            mostrar_cache();
        }
    }
};

// FUNÇÃO PRINCIPAL SIMPLIFICADA
int main() {
    cout << "📚 SISTEMA DE LEITURA - TEXTO É VIDA" << endl;
    cout << "====================================" << endl;
    cout << "✅ Iniciando com algoritmo: FIFO (padrão)" << endl;
    cout << "💡 Digite -1 para simulação e troca automática" << endl;
    
    GerenciadorTextos gerenciador;
    
    int opcao;
    do {
        cout << "\nDigite texto (1-100), -1 simulação, 0 sair: ";
        cin >> opcao;
        
        if (opcao == 0) {
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