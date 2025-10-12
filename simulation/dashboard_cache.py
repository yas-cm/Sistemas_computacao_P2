import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
import numpy as np
import json
import os

# ✅ CONFIGURAR FONTES COMPATÍVEIS ANTES DE QUALQUER OUTRO CÓDIGO
plt.rcParams['font.family'] = 'DejaVu Sans'
plt.rcParams['font.sans-serif'] = ['DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# Configurações de cores
CORES = {
    'fundo_radial': ['#0d002a', '#1a0563'],
    'card': '#4a3b83',
    'borda': '#9b7cff',
    'texto': 'white',
    'grafico_1': '#ff6b6b',
    'grafico_2': '#4ecdc4',
    'grafico_3': '#45b7d1',
    'destaque': '#1fd8b4'
}

def carregar_dados_simulacao():
    """Carrega os dados da simulação do arquivo JSON da pasta docs/"""
    try:
        with open('docs/resultados.json', 'r', encoding='utf-8') as f:
            dados = json.load(f)
        print("Dados da simulação carregados do JSON!")
        return dados, True
    except FileNotFoundError:
        print("Arquivo docs/resultados.json não encontrado!")
        print("Execute a simulação no programa C++ primeiro (digite -1)")
        return None, False
    except json.JSONDecodeError as e:
        print(f"Erro ao ler JSON: {e}")
        print("Recriando arquivo JSON com dados de exemplo...")
        return criar_dados_exemplo(), True
    except Exception as e:
        print(f"Erro inesperado: {e}")
        return criar_dados_exemplo(), True

def criar_dados_exemplo():
    """Cria dados de exemplo se o JSON estiver corrompido"""
    print("Usando dados de exemplo...")
    return {
        "data_simulacao": "2025-10-11 18:51:42",
        "resultados": [
            {
                "algoritmo": "FIFO",
                "tempo_medio": 53.04,
                "taxa_hit": 15.22,
                "cache_misses": 763,
                "total_hits": 137
            },
            {
                "algoritmo": "LRU", 
                "tempo_medio": 52.82,
                "taxa_hit": 16.00,
                "cache_misses": 756,
                "total_hits": 144
            },
            {
                "algoritmo": "2Q",
                "tempo_medio": 50.45,
                "taxa_hit": 19.44,
                "cache_misses": 725,
                "total_hits": 175
            }
        ],
        "info_geral": {
            "total_testes": 900,
            "tamanho_cache": 10,
            "algoritmos_testados": 3,
            "metodos_acesso": ["Uniforme", "Poisson", "Ponderado"]
        }
    }

def processar_dados(dados):
    """Processa os dados do JSON para o dashboard"""
    resultados = dados['resultados']
    
    # Extrair dados básicos
    algoritmos = [r['algoritmo'] for r in resultados]
    tempo_medio = [r['tempo_medio'] for r in resultados]
    taxa_hit = [r['taxa_hit'] for r in resultados]
    cache_misses = [r['cache_misses'] for r in resultados]
    total_hits = [r['total_hits'] for r in resultados]
    
    # Encontrar melhor algoritmo (maior taxa de hit)
    melhor_idx = taxa_hit.index(max(taxa_hit))
    pior_taxa = min(taxa_hit)
    eficiencia = ((taxa_hit[melhor_idx] - pior_taxa) / pior_taxa) * 100
    
    recomendacao = {
        "algoritmo": algoritmos[melhor_idx],
        "tempo_medio": tempo_medio[melhor_idx],
        "taxa_hit": taxa_hit[melhor_idx],
        "cache_misses": cache_misses[melhor_idx],
        "eficiencia": f"+{eficiencia:.0f}%"
    }
    
    # Gerar rankings
    indices_hit = sorted(range(len(taxa_hit)), key=lambda i: taxa_hit[i], reverse=True)
    ranking_hit = [algoritmos[i] for i in indices_hit]
    
    indices_tempo = sorted(range(len(tempo_medio)), key=lambda i: tempo_medio[i])
    ranking_tempo = [algoritmos[i] for i in indices_tempo]
    
    ranking = [
        ["Hit Rate"] + ranking_hit,
        ["Tempo"] + ranking_tempo,
        ["Eficiência"] + [algoritmos[melhor_idx]] + [a for a in algoritmos if a != algoritmos[melhor_idx]]
    ]
    
    info_geral = dados['info_geral']
    
    return {
        'algoritmos': algoritmos,
        'tempo_medio': tempo_medio,
        'taxa_hit': taxa_hit,
        'cache_misses': cache_misses,
        'total_hits': total_hits,
        'recomendacao': recomendacao,
        'ranking': ranking,
        'info_geral': info_geral,
        'data_simulacao': dados.get('data_simulacao', 'Simulação concluída')
    }


def criar_card(ax, titulo=None):
    """Cria um card estilizado para o dashboard"""
    ax.set_facecolor(CORES['card'])
    ax.patch.set_alpha(0.75)
    for spine in ax.spines.values():
        spine.set_color(CORES['borda'])
        spine.set_linewidth(1)
    ax.tick_params(colors=CORES['texto'])
    for label in ax.get_xticklabels() + ax.get_yticklabels():
        label.set_color(CORES['texto'])
    if titulo:
        ax.set_title(titulo, color=CORES['texto'], fontsize=12, pad=12, weight='bold')
    ax.axis('off')
    return ax

def criar_dashboard(dados_processados):
    """Cria o dashboard visual com os dados processados"""
    
    algoritmos = dados_processados['algoritmos']
    tempo_medio = dados_processados['tempo_medio']
    taxa_hit = dados_processados['taxa_hit']
    cache_misses = dados_processados['cache_misses']
    recomendacao = dados_processados['recomendacao']
    ranking = dados_processados['ranking']
    info_geral = dados_processados['info_geral']
    
    # ✅ CORRIGIDO: Usar GridSpec sem tight_layout problemático
    fig = plt.figure(figsize=(16, 9), facecolor=CORES['fundo_radial'][0])
    
    # ✅ CORRIGIDO: Layout manual em vez de tight_layout
    fig.subplots_adjust(left=0.05, right=0.95, bottom=0.05, top=0.93, 
                       wspace=0.15, hspace=0.08)
    

    # Fundo gradiente
    ax_bg = fig.add_axes([0, 0, 1, 1], facecolor='none')
    x = np.linspace(0, 1, 100)
    y = np.linspace(0, 1, 100)
    X, Y = np.meshgrid(x, y)
    Z = np.sqrt((X-0.5)**2 + (Y-0.5)**2)
    ax_bg.imshow(Z, extent=[0, 1, 0, 1], aspect='auto', cmap=plt.cm.Purples, alpha=0.3)
    ax_bg.axis('off')

    # ✅ CORRIGIDO: Grid mais simples e compatível
    # Header
    ax_header = fig.add_axes([0.05, 0.88, 0.90, 0.08])
    criar_card(ax_header)
    ax_header.text(0.5, 0.7, f"RELATORIO DE PERFORMANCE - {recomendacao['algoritmo']} VENCEDOR", 
                   ha='center', va='center', color=CORES['destaque'], fontsize=16, weight='bold')
    ax_header.text(0.5, 0.3, f"Simulacao: {dados_processados['data_simulacao']}", 
                   ha='center', va='center', color=CORES['texto'], fontsize=10)

    # Melhor Algoritmo - ✅ CORRIGIDO: Substituir emojis por texto
    ax_melhor_algo = fig.add_axes([0.05, 0.63, 0.25, 0.23])
    criar_card(ax_melhor_algo)
    ax_melhor_algo.text(0.5, 0.5, 
        f"MELHOR ALGORITMO\n{recomendacao['algoritmo']}\n\n"
        f"* Tempo: {recomendacao['tempo_medio']:.1f}ms\n"
        f"* Hit Rate: {recomendacao['taxa_hit']:.1f}%\n"
        f"* Misses: {recomendacao['cache_misses']}\n"
        f"* Eficiencia: {recomendacao['eficiencia']}",
        fontsize=13, ha='center', va='center', color=CORES['texto'], weight='bold', zorder=1)

    # Info Geral - ✅ CORRIGIDO: Substituir emojis por texto
    ax_info_geral = fig.add_axes([0.05, 0.35, 0.25, 0.23])
    criar_card(ax_info_geral)
    ax_info_geral.text(0.5, 0.5,
        f"INFORMACOES GERAIS\n\n"
        f"Total de testes: {info_geral['total_testes']}\n"
        f"Tamanho do cache: {info_geral['tamanho_cache']}\n"
        f"Algoritmos: {info_geral['algoritmos_testados']}\n"
        f"Metodos de acesso:\n"
        f"  - {info_geral['metodos_acesso'][0]}\n"
        f"  - {info_geral['metodos_acesso'][1]}\n"
        f"  - {info_geral['metodos_acesso'][2]}",
        fontsize=11, ha='center', va='center', color=CORES['texto'], zorder=1)

    # Gráfico de Linha - Tempo
    ax_chart_main = fig.add_axes([0.33, 0.35, 0.40, 0.51])
    criar_card(ax_chart_main)
    ax_chart_main.axis('on')
    x_pos = np.arange(len(algoritmos))
    ax_chart_main.plot(x_pos, tempo_medio, marker='o', color=CORES['grafico_1'], 
                      linewidth=3, markersize=8, markerfacecolor='white')
    ax_chart_main.set_xticks(x_pos)
    ax_chart_main.set_xticklabels(algoritmos)
    ax_chart_main.set_ylabel('Tempo Medio (ms)', color=CORES['texto'], fontsize=11)
    ax_chart_main.grid(True, linestyle='--', alpha=0.3, color=CORES['texto'])
    ax_chart_main.set_facecolor(CORES['card'])
    
    # Gráfico de Barras - Hit Rate
    ax_chart_bar = fig.add_axes([0.75, 0.63, 0.20, 0.23])
    criar_card(ax_chart_bar, 'Taxa de Hit (%)')
    ax_chart_bar.axis('on')
    colors_barras = [CORES['grafico_1'], CORES['grafico_2'], CORES['grafico_3']]
    bars = ax_chart_bar.bar(x_pos, taxa_hit, color=colors_barras)
    ax_chart_bar.set_xticks(x_pos)
    ax_chart_bar.set_xticklabels(algoritmos)
    ax_chart_bar.set_ylabel('Taxa (%)', color=CORES['texto'], fontsize=10)
    for i, v in enumerate(taxa_hit):
        ax_chart_bar.text(i, v + 0.3, f"{v:.1f}%", ha='center', va='bottom', 
                         fontsize=9, color=CORES['texto'], weight='bold')
    ax_chart_bar.set_facecolor(CORES['card'])

    # Gráfico de Pizza - Cache Misses
    ax_chart_pie = fig.add_axes([0.75, 0.35, 0.20, 0.23])
    criar_card(ax_chart_pie)
    ax_chart_pie.axis('on')
    ax_chart_pie.text(0, 1.25, 'Cache Misses', fontsize=11, ha='center', 
                   va='center', color=CORES['texto'], weight='bold')
    colors_pie = [CORES['grafico_1'], CORES['grafico_2'], CORES['grafico_3']]
    wedges, texts, autotexts = ax_chart_pie.pie(cache_misses, labels=algoritmos, autopct='%1.1f%%', 
                                               colors=colors_pie, startangle=90, 
                                               textprops={'color': CORES['texto'], 'fontsize': 9})
    for autotext in autotexts:
        autotext.set_color('white')
        autotext.set_weight('bold')

    # Tabela de Performance
    ax_tabela1 = fig.add_axes([0.05, 0.05, 0.55, 0.25])
    criar_card(ax_tabela1)
    ax_tabela1.text(0.5, 0.92, 'Detalhes de Performance', fontsize=11, ha='center', 
                   va='center', color=CORES['texto'], weight='bold')
    
    tabela1_dados = [["Algoritmo", "Hit Rate", "Tempo", "Misses", "Hits"]]
    for i, algo in enumerate(algoritmos):
        tabela1_dados.append([
            algo,
            f"{taxa_hit[i]:.1f}%",
            f"{tempo_medio[i]:.1f}ms",
            f"{cache_misses[i]}",
            f"{dados_processados['total_hits'][i]}"
        ])
    
    table1 = ax_tabela1.table(cellText=tabela1_dados, loc='center', 
                             cellLoc='center', bbox=[0.05, 0.15, 0.9, 0.7])
    table1.auto_set_font_size(False)
    table1.set_fontsize(9)
    for (i, j), cell in table1.get_celld().items():
        if i == 0:
            cell.set_facecolor(CORES['borda'])
            cell.set_text_props(weight='bold', color='white')
        else:
            cell.set_facecolor(CORES['card'])
            cell.set_text_props(color='white')
        cell.set_edgecolor(CORES['borda'])
        cell.set_height(0.15)

    # Tabela de Rankings
    ax_tabela2 = fig.add_axes([0.63, 0.05, 0.32, 0.25])
    criar_card(ax_tabela2)
    ax_tabela2.text(0.5, 0.92, 'Rankings Comparativos', fontsize=11, ha='center', 
                   va='center', color=CORES['texto'], weight='bold')
    
    table2 = ax_tabela2.table(cellText=ranking, loc='center', 
                             cellLoc='center', bbox=[0.1, 0.15, 0.8, 0.7])
    table2.auto_set_font_size(False)
    table2.set_fontsize(9)
    for (i, j), cell in table2.get_celld().items():
        cell.set_facecolor(CORES['card'])
        cell.set_text_props(color='white')
        cell.set_edgecolor(CORES['borda'])
        cell.set_height(0.15)

    # ✅ CORRIGIDO: Não usar tight_layout()
    print("Dashboard gerado com sucesso!")
    plt.show()  # <-- Adicione esta linha

def main():
    """Função principal"""
    print("INICIANDO DASHBOARD DE ANALISE DE CACHE...")
    print("=" * 50)
    
    # Carregar dados
    dados, sucesso = carregar_dados_simulacao()
    
    if not sucesso:
        print("NAO FOI POSSIVEL CARREGAR OS DADOS DA SIMULACAO.")
        return
    
    # Processar dados
    dados_processados = processar_dados(dados)
    
    # Exibir resumo no console - ✅ CORRIGIDO: Sem emojis
    print(f"\nALGORITMO VENCEDOR: {dados_processados['recomendacao']['algoritmo']}")
    print(f"Hit Rate: {dados_processados['recomendacao']['taxa_hit']:.1f}%")
    print(f"Eficiencia: {dados_processados['recomendacao']['eficiencia']}")
    print(f"Tempo Medio: {dados_processados['recomendacao']['tempo_medio']:.1f}ms")
    print(f"Total de Testes: {dados_processados['info_geral']['total_testes']}")
    
    # Criar dashboard
    print("\nGERANDO DASHBOARD VISUAL...")
    criar_dashboard(dados_processados)
    
    print("\nDASHBOARD CONCLUIDO! Feche a janela para continuar.")

if __name__ == "__main__":
    main()