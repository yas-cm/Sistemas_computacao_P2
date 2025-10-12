# Sistema de Cache para Leitura de Textos

## 📋 Descrição do Projeto

Sistema desenvolvido para a empresa Texto é Vida que implementa algoritmos de cache para otimizar o tempo de carregamento de textos em um sistema de discos forense lento. Gerencia 100 textos com mais de 1000 palavras cada, utilizando os algoritmos de substituição de cache: **FIFO**, **LRU** e **2Q**.

## 🏗️ Arquitetura do Projeto

### Estrutura de Diretórios

```bash
GRUPO3_RA2/
├── algorithms/
│   ├── algoritmo_cache.cpp
│   ├── cache_fifo.cpp
│   ├── cache_lru.cpp
│   └── cache_2q.cpp
├── core/
├── simulation/
│   ├── simulador.cpp
│   └── dashboard_cache.py
├── texts/
│   ├── 1.txt a 100.txt
├── docs/
│   ├── resultados.json
│   └── relatorio_simulacao.txt
├── start_tudo.bat
├── ra2_main.cpp
└── README.md
```

## 🔧 Algoritmos Implementados

### 1. FIFO (First-In, First-Out)

-   Remove o texto mais antigo do cache
-   Implementação com queue + unordered_map
-   Busca O(1)

### 2. LRU (Least Recently Used)

-   Remove o texto menos recentemente usado
-   Estrutura list + unordered_map
-   Acesso O(1) com atualização de posição

### 3. 2Q (Two Queues)

-   Combina FIFO para novos acessos e LRU para frequentes
-   Prevenção de poluição do cache
-   Melhor performance geral

---

## 🚀 Como Compilar e Executar

### Pré-requisitos

-   Compilador C++ (g++ no Windows)
-   Python 3.x com matplotlib, numpy e json
-   100 arquivos de texto na pasta texts/

### Compilação

```bash
start_tudo.bat
```

---

## 🎯 Utilização

Por padrão, o algoritmo a ser utilizado será o **FIFO**.
Exemplo de utilização:

```bash
Digite texto (1-100), -1 simulacao, 0 sair: 42
[CACHE MISS] Carregando texto 42 do disco...
Texto 42 (primeiras linhas):
==========================================
[conteúdo...]
==========================================
Tempo: 150ms
CACHE ATUAL: [42] (1/10)
```

### Modo Simulação

Acesse com -1 para executar a simulação completa:

-   3 usuários com 300 requisições cada
-   3 padrões de acesso: Aleatório, Poisson, Ponderado
-   Dashboard visual com resultados comparativos

Após executada, o programa irá abrir automáticamente a dashboard dos resultados.
Para retornar ao programa de leitura dos textos, feche a dashboard e retorne ao cmd.
O programa irá mudar automaticamente para o algoritmo mais rápido, decidido pela simulação.
