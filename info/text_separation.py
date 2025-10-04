import os
import re
import glob

def processar_livro(arquivo_entrada, palavras_por_arquivo=1000):
    # Criar pasta 'textos' se não existir
    pasta_textos = 'textos'
    if not os.path.exists(pasta_textos):
        os.makedirs(pasta_textos)
    
    # Encontrar o último número usado
    ultimo_numero = encontrar_ultimo_numero(pasta_textos)
    print(f"Continuando a partir do arquivo número: {ultimo_numero}")
    
    # Ler o arquivo original
    with open(arquivo_entrada, 'r', encoding='utf-8') as file:
        texto_completo = file.read()
    
    # Limpar o texto - remover partes indesejadas
    texto_limpo = limpar_texto(texto_completo)
    
    # Dividir em palavras
    palavras = texto_limpo.split()
    
    # Dividir em chunks de 1000 palavras
    chunks = []
    for i in range(0, len(palavras), palavras_por_arquivo):
        chunk = palavras[i:i + palavras_por_arquivo]
        chunks.append(' '.join(chunk))
    
    # Salvar cada chunk em arquivos numerados (continuando do último)
    arquivos_criados = 0
    for i, chunk in enumerate(chunks, ultimo_numero + 1):
        nome_arquivo = os.path.join(pasta_textos, f'{i}.txt')
        with open(nome_arquivo, 'w', encoding='utf-8') as file:
            file.write(chunk)
        arquivos_criados += 1
    
    print(f'Novos arquivos criados: {arquivos_criados}')
    print(f'Pasta de destino: {pasta_textos}')
    print(f'Próximo número disponível: {ultimo_numero + arquivos_criados + 1}')

def encontrar_ultimo_numero(pasta):
    """
    Encontra o maior número de arquivo na pasta
    """
    # Padrão para arquivos .txt com apenas números no nome
    padrao = os.path.join(pasta, '*.txt')
    arquivos = glob.glob(padrao)
    
    numeros = []
    for arquivo in arquivos:
        # Pega apenas o nome do arquivo (sem extensão)
        nome_base = os.path.splitext(os.path.basename(arquivo))[0]
        # Verifica se é um número
        if nome_base.isdigit():
            numeros.append(int(nome_base))
    
    return max(numeros) if numeros else 0

def limpar_texto(texto):
    """
    Remove partes não desejadas do texto como capa, índice, etc.
    """
    # Converter para minúsculas para facilitar a busca
    texto_lower = texto.lower()
    
    # Padrões comuns para identificar início do conteúdo real
    padroes_inicio = [
        r'.*?(cap[íi]tulo\s+[i1]|chapter\s+1|introdu[çc][aã]o|pr[oó]logo)',
        r'.*?(come[çc]o\s+da\s+narrativa|in[íi]cio\s+da\s+hist[óo]ria)',
        r'.*\n{3,}.*?\n{3,}'  # Múltiplas quebras de linha consecutivas
    ]
    
    # Encontrar o início do conteúdo real
    inicio_conteudo = 0
    for padrao in padroes_inicio:
        match = re.search(padrao, texto_lower, re.IGNORECASE | re.DOTALL)
        if match:
            inicio_conteudo = match.start()
            print(f"Encontrado início do conteúdo em: {match.group()}")
            break
    
    # Se não encontrou padrão específico, tenta encontrar onde começa o texto contínuo
    if inicio_conteudo == 0:
        # Procura por parágrafos longos (indicando conteúdo real)
        linhas = texto.split('\n')
        for i, linha in enumerate(linhas):
            if len(linha.strip()) > 100:  # Linha com mais de 100 caracteres
                inicio_conteudo = texto.find(linha)
                print(f"Início estimado do conteúdo na linha {i+1}")
                break
    
    # Extrair apenas o conteúdo a partir do ponto encontrado
    if inicio_conteudo > 0:
        texto = texto[inicio_conteudo:]
    
    # Remover números de página (padrões comuns)
    texto = re.sub(r'\n\s*\d+\s*\n', '\n', texto)  # Números de página isolados
    texto = re.sub(r'^\d+\s*$', '', texto, flags=re.MULTILINE)  # Linhas só com números
    
    # Remover headers/footers repetitivos
    linhas = texto.split('\n')
    linhas_unicas = []
    ultima_linha = ""
    
    for linha in linhas:
        linha_limpa = linha.strip()
        if (linha_limpa and 
            linha_limpa != ultima_linha and 
            len(linha_limpa) > 2 and  # Ignora linhas muito curtas
            not linha_limpa.isspace()):
            linhas_unicas.append(linha)
            ultima_linha = linha_limpa
    
    texto = '\n'.join(linhas_unicas)
    
    # Remover múltiplos espaços e quebras de linha excessivas
    texto = re.sub(r'\n{3,}', '\n\n', texto)
    texto = re.sub(r' +', ' ', texto)
    
    return texto.strip()

def listar_arquivos_existentes(pasta):
    """Lista os arquivos existentes na pasta"""
    padrao = os.path.join(pasta, '*.txt')
    arquivos = glob.glob(padrao)
    arquivos_numeros = []
    
    for arquivo in arquivos:
        nome_base = os.path.splitext(os.path.basename(arquivo))[0]
        if nome_base.isdigit():
            arquivos_numeros.append(int(nome_base))
    
    arquivos_numeros.sort()
    return arquivos_numeros

def modo_interativo():
    """Modo interativo para ajudar a identificar o início do conteúdo"""
    arquivo = input("Digite o caminho do arquivo txt: ")
    
    if not os.path.exists(arquivo):
        print("Arquivo não encontrado!")
        return
    
    # Mostrar arquivos existentes
    pasta_textos = 'textos'
    if os.path.exists(pasta_textos):
        arquivos_existentes = listar_arquivos_existentes(pasta_textos)
        if arquivos_existentes:
            print(f"\nArquivos existentes na pasta '{pasta_textos}': {arquivos_existentes}")
            print(f"Último número: {max(arquivos_existentes)}")
        else:
            print(f"\nPasta '{pasta_textos}' vazia. Começando do 1.")
    
    with open(arquivo, 'r', encoding='utf-8') as file:
        conteudo = file.read()
    
    print("\n=== PRIMEIRAS 20 LINHAS DO ARQUIVO ===")
    linhas = conteudo.split('\n')[:20]
    for i, linha in enumerate(linhas, 1):
        print(f"{i:2d}: {linha[:100]}{'...' if len(linha) > 100 else ''}")
    
    resposta = input("\nDeseja processar o arquivo? (s/n): ")
    if resposta.lower() == 's':
        processar_livro(arquivo)
    else:
        print("Processamento cancelado.")

# Execução principal
if __name__ == "__main__":
    print("=== DIVISOR DE LIVROS EM TEXTOS MENORES ===\n")
    
    opcao = input("Escolha uma opção:\n1 - Modo interativo\n2 - Processar arquivo direto\nOpção: ")
    
    if opcao == "1":
        modo_interativo()
    elif opcao == "2":
        arquivo = input("Digite o caminho do arquivo txt: ")
        if os.path.exists(arquivo):
            processar_livro(arquivo)
        else:
            print("Arquivo não encontrado!")
    else:
        print("Opção inválida!")