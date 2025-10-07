import matplotlib.pyplot as plt
import numpy as np
import os
from datetime import datetime

def ler_dados_simulacao():
    """Lê os dados da simulação do arquivo"""
    try:
        with open('simulation/resultados_simulacao.txt', 'r') as f:
            linhas = f.readlines()
        
        dados = {}
        for linha in linhas:
            if ':' in linha:
                chave, valores = linha.split(':', 1)
                dados[chave.strip()] = [x.strip() for x in valores.strip().split()]
        
        # Converter para tipos apropriados
        algoritmos = dados.get('ALGORITMOS', ['FIFO', 'LRU', 'MRU'])
        tempo_medio = [float(x) for x in dados.get('TEMPO_MEDIO', ['125', '98', '110'])]
        taxa_hit = [float(x) for x in dados.get('TAXA_HIT', ['60', '85', '75'])]
        total_misses = [int(x) for x in dados.get('TOTAL_MISSES', ['240', '86', '150'])]
        
        return {
            'algoritmos': algoritmos,
            'tempo_medio': tempo_medio,
            'taxa_hit': taxa_hit,
            'total_misses': total_misses
        }
    
    except FileNotFoundError:
        print("Arquivo de resultados nao encontrado. Usando dados de exemplo.")
        return {
            'algoritmos': ['FIFO', 'LRU', 'MRU'],
            'tempo_medio': [125, 98, 110],
            'taxa_hit': [60, 85, 75],
            'total_misses': [240, 86, 150]
        }

def gerar_grafico_unico():
    """Gera um unico arquivo com dois graficos e recomendacao"""
    dados = ler_dados_simulacao()
    
    # Criar figura com 3 subplots: 2 graficos + 1 area para recomendacao
    fig = plt.figure(figsize=(12, 8))
    
    # Definir grid: 2 linhas (80% graficos, 20% recomendacao) e 2 colunas
    gs = plt.GridSpec(3, 2, height_ratios=[0.5, 3.5, 1])
    
    # Grafico 1: Tempo Medio (esquerda superior)
    ax1 = fig.add_subplot(gs[0, 0])
    bars1 = ax1.bar(dados['algoritmos'], dados['tempo_medio'], 
                   color=['#FF6B6B', '#4ECDC4', '#A545D1'])
    
    ax1.set_title('Tempo Medio de Acesso', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Milissegundos (ms)')
    ax1.set_xlabel('Algoritmos')
    ax1.grid(True, alpha=0.3, axis='y')
    
    # Adicionar valores nas barras do tempo
    for bar in bars1:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height + 1,
                f'{height:.1f}ms', ha='center', va='bottom', fontweight='bold')
    
    # Grafico 2: Cache Performance (direita superior)
    ax2 = fig.add_subplot(gs[0, 1])
    x = np.arange(len(dados['algoritmos']))
    largura = 0.35
    
    bars2a = ax2.bar(x - largura/2, dados['taxa_hit'], largura, 
                    label='Taxa de Hit (%)', color="#81EEB0")
    bars2b = ax2.bar(x + largura/2, dados['total_misses'], largura, 
                    label='Cache Misses', color="#F57D95")
    
    ax2.set_title('Performance do Cache', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Valores')
    ax2.set_xlabel('Algoritmos')
    ax2.set_xticks(x)
    ax2.set_xticklabels(dados['algoritmos'])
    ax2.legend()
    ax2.grid(True, alpha=0.3, axis='y')
    
    # Adicionar valores nas barras do cache
    for bars in [bars2a, bars2b]:
        for bar in bars:
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height + 1,
                    f'{height:.0f}{"%" if bars == bars2a else ""}', 
                    ha='center', va='bottom', fontsize=9)
    
    # Area da recomendacao (ocupa as duas colunas inferiores)
    ax3 = fig.add_subplot(gs[1, :])
    ax3.axis('off')
    
    # Encontrar o melhor algoritmo
    melhor_idx = np.argmax(dados['taxa_hit'])
    melhor_algoritmo = dados['algoritmos'][melhor_idx]
    
    # Texto da recomendacao
    recomendacao_texto = (
        f"RECOMENDACAO: {melhor_algoritmo} é a melhor escolha\n\n"
        f"• Tempo medio: {dados['tempo_medio'][melhor_idx]:.1f} ms\n"
        f"• Taxa de cache hit: {dados['taxa_hit'][melhor_idx]:.1f}%\n"
        f"• Cache misses: {dados['total_misses'][melhor_idx]}\n"
        f"• Performance geral: Superior em eficiencia e velocidade"
    )
    
    # Caixinha da recomendacao
    ax3.text(0.5, 0.7, recomendacao_texto, 
             transform=ax3.transAxes, ha='center', va='center',
             fontsize=11, fontweight='bold',
             bbox=dict(boxstyle="round,pad=1.0", 
                      facecolor='lightgreen', 
                      edgecolor='darkgreen',
                      alpha=0.8))
    
    # Titulo principal
    fig.suptitle('Comparacao de Algoritmos de Cache', fontsize=16, fontweight='bold', y=0.95)
    
    # Ajustar layout
    plt.tight_layout()
    
    # Salvar unico arquivo
    caminho_grafico = 'docs/grafico_resultados.png'
    plt.savefig(caminho_grafico, dpi=150, bbox_inches='tight')
    plt.close()
    
    return caminho_grafico

def gerar_relatorio_texto():
    """Gera um relatorio em texto na pasta docs"""
    dados = ler_dados_simulacao()
    
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    melhor_idx = np.argmax(dados['taxa_hit'])
    
    relatorio = f"""
RELATORIO DE SIMULACAO - ALGORITMOS DE CACHE
Data: {timestamp}

RESULTADOS:
-----------
Algoritmo  | Tempo Medio | Taxa de Hit | Cache Misses
---------- | ----------- | ----------- | ------------
{dados['algoritmos'][0]:<9} | {dados['tempo_medio'][0]:<11.1f} | {dados['taxa_hit'][0]:<11.1f} | {dados['total_misses'][0]:<12}
{dados['algoritmos'][1]:<9} | {dados['tempo_medio'][1]:<11.1f} | {dados['taxa_hit'][1]:<11.1f} | {dados['total_misses'][1]:<12}
{dados['algoritmos'][2]:<9} | {dados['tempo_medio'][2]:<11.1f} | {dados['taxa_hit'][2]:<11.1f} | {dados['total_misses'][2]:<12}

RECOMENDACAO:
-------------
Algoritmo escolhido: {dados['algoritmos'][melhor_idx]}
• Tempo medio: {dados['tempo_medio'][melhor_idx]:.1f} ms
• Taxa de cache hit: {dados['taxa_hit'][melhor_idx]:.1f}%
• Total de cache misses: {dados['total_misses'][melhor_idx]}

METODOLOGIA:
------------
• 3 usuarios simulados
• 200 requisicoes por usuario
• 3 padroes de acesso: Aleatorio, Poisson, Ponderado (30-40)
• Cache com capacidade para 10 textos
"""
    
    caminho_relatorio = 'docs/relatorio_simulacao.txt'
    with open(caminho_relatorio, 'w', encoding='utf-8') as f:
        f.write(relatorio)
    
    return caminho_relatorio

def gerar_todos_documentos():
    """Gera todos os documentos"""
    print("Gerando grafico e relatorio...")
    
    grafico = gerar_grafico_unico()
    relatorio = gerar_relatorio_texto()
    
    print(f"Grafico salvo: {grafico}")
    print(f"Relatorio salvo: {relatorio}")
    
    return grafico, relatorio

if __name__ == "__main__":
    gerar_todos_documentos()