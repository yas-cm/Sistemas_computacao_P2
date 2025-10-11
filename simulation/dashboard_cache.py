import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec
from matplotlib.patches import FancyBboxPatch
import numpy as np

# Variáveis do relatório
data_simulacao = "2025-10-07 19:38:06"

resultados = [
    {"algoritmo": "FIFO", "tempo_medio": 54.4, "taxa_hit": 13.3, "cache_misses": 780},
    {"algoritmo": "LRU",  "tempo_medio": 52.9, "taxa_hit": 15.0, "cache_misses": 765},
    {"algoritmo": "2Q",   "tempo_medio": 50.9, "taxa_hit": 18.9, "cache_misses": 730},
]

recomendacao = {
    "algoritmo": "2Q",
    "tempo_medio": 50.9,
    "taxa_hit": 18.9,
    "cache_misses": 730,
    "eficiencia": "+42%"
}

info_geral = {
    "total_testes": 900,
    "tamanho_cache": 10,
    "algoritmos": ", ".join([r["algoritmo"] for r in resultados]),
    "metodos": ["Uniforme", "Poisson", "Ponderado"]
}

ranking = [
    ["Hit Rate", "2Q", "LRU", "FIFO"],
    ["Tempo", "LRU", "2Q", "FIFO"],
    ["Eficiencia", "2Q", "LRU", "FIFO"],
]

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

# Dados extraídos das variáveis
algoritmos = [r["algoritmo"] for r in resultados]
tempo_medio = [r["tempo_medio"] for r in resultados]
taxa_hit = [r["taxa_hit"] for r in resultados]
cache_misses = [r["cache_misses"] for r in resultados]

fig = plt.figure(figsize=(16, 9), facecolor=CORES['fundo_radial'][0])
fig.suptitle("Dashboard de Simulação - Algoritmos de Cache", fontsize=20, weight='bold', color=CORES['texto'], y=0.98)
ax_bg = fig.add_axes([0, 0, 1, 1], facecolor='none')
x = np.linspace(0, 1, 100)
y = np.linspace(0, 1, 100)
X, Y = np.meshgrid(x, y)
Z = np.sqrt((X-0.5)**2 + (Y-0.5)**2)
ax_bg.imshow(Z, extent=[0, 1, 0, 1], aspect='auto', cmap=plt.cm.Purples, alpha=0.3)
ax_bg.set_xlim(0, 1)
ax_bg.set_ylim(0, 1)
ax_bg.axis('off')

gs_principal = GridSpec(4, 3, figure=fig, width_ratios=[0.3, 0.47, 0.35], height_ratios=[0.12, 0.45, 0.45, 0.4], wspace=0.15, hspace=0.08, left=0.05, right=0.95, bottom=0.05, top=0.925)

def criar_card(ax, titulo=None, alinhamento='center'):
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

ax_header = criar_card(fig.add_subplot(gs_principal[0, :]))
ax_melhor_algo = criar_card(fig.add_subplot(gs_principal[1, 0]))
ax_melhor_algo.axis('on')
ax_melhor_algo.set_xticks([])
ax_melhor_algo.set_yticks([])
ax_melhor_algo.text(
    0.5, 0.5,
    f"Melhor Algoritmo\n{recomendacao['algoritmo']}\n\n"
    f"• Menor tempo: {recomendacao['tempo_medio']}ms\n"
    f"• Maior hit rate: {recomendacao['taxa_hit']}%\n"
    f"• Menor misses: {recomendacao['cache_misses']}\n"
    f"• Eficiência geral: {recomendacao['eficiencia']}",
    fontsize=13, ha='center', va='center', color=CORES['texto'], weight='bold', zorder=1
)
ax_info_geral = criar_card(fig.add_subplot(gs_principal[2, 0]))
ax_info_geral.axis('on')
ax_info_geral.set_xticks([])
ax_info_geral.set_yticks([])
ax_info_geral.text(
    0.5, 0.5,
    f"Info Geral\n\n"
    f"Total de testes: {info_geral['total_testes']}\n"
    f"Tamanho do cache: {info_geral['tamanho_cache']}\n"
    f"Algoritmos: {info_geral['algoritmos']}\n"
    f"Métodos de aleatoriedade:\n  - " + "\n  - ".join(info_geral['metodos']),
    fontsize=11, ha='center', va='center', color=CORES['texto'], zorder=1
)

ax_chart_main = criar_card(fig.add_subplot(gs_principal[1:3, 1]), 'Tempo de Resposta')
ax_chart_main.axis('on')
pos = ax_chart_main.get_position()
ax_chart_main.set_position([pos.x0 + 0.02, pos.y0, pos.width, pos.height])
x_pos = np.arange(len(algoritmos))
ax_chart_main.plot(x_pos, tempo_medio, marker='o', color=CORES['grafico_1'], linewidth=3, markersize=8, markerfacecolor='white')
ax_chart_main.set_xticks(x_pos)
ax_chart_main.set_xticklabels(algoritmos)
ax_chart_main.set_ylabel('Tempo Médio (ms)', color=CORES['texto'], fontsize=11)
ax_chart_main.grid(True, linestyle='--', alpha=0.3, color=CORES['texto'])
ax_chart_main.set_facecolor(CORES['card'])

ax_chart_bar = criar_card(fig.add_subplot(gs_principal[1, 2]), 'Hit Rate')
ax_chart_bar.axis('on')
colors_barras = [CORES['grafico_1'], CORES['grafico_2'], CORES['grafico_3']]
bars = ax_chart_bar.bar(x_pos, taxa_hit, color=colors_barras)
ax_chart_bar.set_xticks(x_pos)
ax_chart_bar.set_xticklabels(algoritmos)
ax_chart_bar.set_ylabel('Taxa (%)', color=CORES['texto'], fontsize=10)
for i, v in enumerate(taxa_hit):
    ax_chart_bar.text(i, v + 0.3, f"{v}%", ha='center', va='bottom', fontsize=9, color=CORES['texto'], weight='bold')
ax_chart_bar.set_facecolor(CORES['card'])

ax_chart_pie = criar_card(fig.add_subplot(gs_principal[2, 2]))
ax_chart_pie.axis('on')
ax_chart_pie.set_ylabel('Cache Misses', color=CORES['texto'], fontsize=9, style='italic', labelpad=18)
colors_pie = [CORES['grafico_1'], CORES['grafico_2'], CORES['grafico_3']]
wedges, texts, autotexts = ax_chart_pie.pie(cache_misses, labels=algoritmos, autopct='%1.1f%%', colors=colors_pie, startangle=90, textprops={'color': CORES['texto']})
for autotext in autotexts:
    autotext.set_color('white')
    autotext.set_weight('bold')
ax_chart_pie.set_facecolor(CORES['card'])

ax_tabela1 = criar_card(fig.add_subplot(gs_principal[3, :2]))
ax_tabela1.text(0.5, 0.88, 'Detalhes de Performance', fontsize=10, ha='center', va='center', color=CORES['texto'], style='italic')
tabela1_dados = [
    ["Algoritmo", "Hit Rate", "Tempo", "Misses", "+ vs Pior"],
    [resultados[0]["algoritmo"], f"{resultados[0]['taxa_hit']}%", f"{resultados[0]['tempo_medio']}ms", f"{resultados[0]['cache_misses']}", "Base"],
    [resultados[1]["algoritmo"], f"{resultados[1]['taxa_hit']}%", f"{resultados[1]['tempo_medio']}ms", f"{resultados[1]['cache_misses']}", "+12%"],
    [resultados[2]["algoritmo"], f"{resultados[2]['taxa_hit']}%", f"{resultados[2]['tempo_medio']}ms", f"{resultados[2]['cache_misses']}", recomendacao["eficiencia"]],
]
table1 = ax_tabela1.table(cellText=tabela1_dados, loc='center', cellLoc='center', bbox=[0.05, 0.15, 0.9, 0.7])
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

ax_tabela2 = criar_card(fig.add_subplot(gs_principal[3, 2]))
ax_tabela2.text(0.5, 0.88, 'Rankings Comparativos por Métrica', fontsize=10, ha='center', va='center', color=CORES['texto'], style='italic')
col_labels = ["Métrica", "1º Lugar", "2º Lugar", "3º Lugar"]
table2 = ax_tabela2.table(cellText=ranking, colLabels=col_labels, loc='center', cellLoc='center', bbox=[0.1, 0.15, 0.8, 0.7])
table2.auto_set_font_size(False)
table2.set_fontsize(9)
for (i, j), cell in table2.get_celld().items():
    if i == 0:
        cell.set_facecolor(CORES['borda'])
        cell.set_text_props(weight='bold', color='white')
    else:
        cell.set_facecolor(CORES['card'])
        cell.set_text_props(color='white')
    cell.set_edgecolor(CORES['borda'])
    cell.set_height(0.15)

plt.subplots_adjust(left=0.05, right=0.95, bottom=0.05, top=1, wspace=0.08, hspace=0.08)
plt.show()
