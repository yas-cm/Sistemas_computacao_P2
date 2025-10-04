import os
import glob

def verificar_palavras_arquivos(pasta_textos='textos', minimo_palavras=1000):
    """
    Verifica se todos os arquivos de texto na pasta têm pelo menos o número mínimo de palavras
    """
    if not os.path.exists(pasta_textos):
        print(f"❌ Pasta '{pasta_textos}' não encontrada!")
        return
    
    # Padrão para arquivos .txt
    padrao = os.path.join(pasta_textos, '*.txt')
    arquivos = glob.glob(padrao)
    
    if not arquivos:
        print(f"❌ Nenhum arquivo .txt encontrado na pasta '{pasta_textos}'")
        return
    
    print(f"🔍 Verificando {len(arquivos)} arquivos na pasta '{pasta_textos}'...")
    print(f"📊 Mínimo requerido: {minimo_palavras} palavras por arquivo\n")
    
    arquivos_com_problema = []
    total_palavras = 0
    estatisticas = []
    
    for arquivo in sorted(arquivos, key=lambda x: extrair_numero(x)):
        nome_arquivo = os.path.basename(arquivo)
        try:
            with open(arquivo, 'r', encoding='utf-8') as file:
                conteudo = file.read()
            
            palavras = conteudo.split()
            num_palavras = len(palavras)
            total_palavras += num_palavras
            
            status = "✅ OK" if num_palavras >= minimo_palavras else "❌ ABAIXO"
            
            if num_palavras < minimo_palavras:
                arquivos_com_problema.append((nome_arquivo, num_palavras))
            
            estatisticas.append((nome_arquivo, num_palavras, status))
            
            # Mostrar resultado individual
            print(f"{status} {nome_arquivo}: {num_palavras} palavras")
            
        except Exception as e:
            print(f"❌ ERRO ao ler {nome_arquivo}: {str(e)}")
            arquivos_com_problema.append((nome_arquivo, f"Erro: {str(e)}"))
    
    # Estatísticas gerais
    print(f"\n{'='*50}")
    print("📈 ESTATÍSTICAS GERAIS:")
    print(f"{'='*50}")
    
    media_palavras = total_palavras / len(arquivos) if arquivos else 0
    arquivos_ok = len(arquivos) - len(arquivos_com_problema)
    
    print(f"📁 Total de arquivos: {len(arquivos)}")
    print(f"✅ Arquivos com ≥ {minimo_palavras} palavras: {arquivos_ok}")
    print(f"❌ Arquivos com menos de {minimo_palavras} palavras: {len(arquivos_com_problema)}")
    print(f"📊 Média de palavras por arquivo: {media_palavras:.1f}")
    print(f"📦 Total de palavras na pasta: {total_palavras:,}")
    
    # Mostrar arquivos com problemas
    if arquivos_com_problema:
        print(f"\n⚠️  ARQUIVOS COM POUCAS PALAVRAS:")
        print(f"{'='*50}")
        for nome_arquivo, num_palavras in arquivos_com_problema:
            if isinstance(num_palavras, int):
                print(f"❌ {nome_arquivo}: {num_palavras} palavras (faltam {minimo_palavras - num_palavras})")
            else:
                print(f"❌ {nome_arquivo}: {num_palavras}")
    
    # Arquivo com mais e menos palavras
    if estatisticas and all(isinstance(e[1], int) for e in estatisticas):
        maior_arquivo = max(estatisticas, key=lambda x: x[1])
        menor_arquivo = min(estatisticas, key=lambda x: x[1])
        
        print(f"\n🎯 DESTAQUES:")
        print(f"{'='*50}")
        print(f"📈 Maior arquivo: {maior_arquivo[0]} - {maior_arquivo[1]} palavras")
        print(f"📉 Menor arquivo: {menor_arquivo[0]} - {menor_arquivo[1]} palavras")
        print(f"📏 Variação: {maior_arquivo[1] - menor_arquivo[1]} palavras de diferença")
    
    return len(arquivos_com_problema) == 0

def extrair_numero(caminho_arquivo):
    """
    Extrai o número do nome do arquivo para ordenação
    """
    nome_base = os.path.splitext(os.path.basename(caminho_arquivo))[0]
    try:
        return int(nome_base)
    except ValueError:
        return nome_base

def verificar_arquivo_especifico(caminho_arquivo, minimo_palavras=1000):
    """
    Verifica um arquivo específico
    """
    if not os.path.exists(caminho_arquivo):
        print(f"❌ Arquivo '{caminho_arquivo}' não encontrado!")
        return
    
    try:
        with open(caminho_arquivo, 'r', encoding='utf-8') as file:
            conteudo = file.read()
        
        palavras = conteudo.split()
        num_palavras = len(palavras)
        
        print(f"📄 Arquivo: {os.path.basename(caminho_arquivo)}")
        print(f"📊 Palavras: {num_palavras}")
        print(f"🎯 Mínimo requerido: {minimo_palavras}")
        
        if num_palavras >= minimo_palavras:
            print("✅ STATUS: OK - Atende ao mínimo requerido")
            return True
        else:
            print(f"❌ STATUS: ABAIXO - Faltam {minimo_palavras - num_palavras} palavras")
            return False
            
    except Exception as e:
        print(f"❌ ERRO ao ler arquivo: {str(e)}")
        return False

def modo_interativo():
    """
    Modo interativo para verificação
    """
    print("=== VERIFICADOR DE CONTAGEM DE PALAVRAS ===\n")
    
    print("Opções:")
    print("1 - Verificar toda a pasta 'textos'")
    print("2 - Verificar um arquivo específico")
    print("3 - Definir número mínimo personalizado")
    
    opcao = input("\nEscolha uma opção: ")
    
    if opcao == "1":
        minimo = input(f"Digite o mínimo de palavras (padrão: 1000): ").strip()
        minimo_palavras = int(minimo) if minimo.isdigit() else 1000
        verificar_palavras_arquivos('textos', minimo_palavras)
        
    elif opcao == "2":
        arquivo = input("Digite o caminho do arquivo: ").strip()
        minimo = input(f"Digite o mínimo de palavras (padrão: 1000): ").strip()
        minimo_palavras = int(minimo) if minimo.isdigit() else 1000
        verificar_arquivo_especifico(arquivo, minimo_palavras)
        
    elif opcao == "3":
        pasta = input("Digite o nome da pasta (padrão: 'textos'): ").strip() or 'textos'
        minimo = input("Digite o número mínimo de palavras: ").strip()
        if minimo.isdigit():
            verificar_palavras_arquivos(pasta, int(minimo))
        else:
            print("❌ Número inválido!")
    else:
        print("❌ Opção inválida!")

# Execução principal
if __name__ == "__main__":
    import sys
    
    if len(sys.argv) > 1:
        # Modo linha de comando
        if sys.argv[1] == "--file":
            arquivo = sys.argv[2] if len(sys.argv) > 2 else input("Digite o caminho do arquivo: ")
            minimo = int(sys.argv[3]) if len(sys.argv) > 3 else 1000
            verificar_arquivo_especifico(arquivo, minimo)
        elif sys.argv[1] == "--folder":
            pasta = sys.argv[2] if len(sys.argv) > 2 else 'textos'
            minimo = int(sys.argv[3]) if len(sys.argv) > 3 else 1000
            verificar_palavras_arquivos(pasta, minimo)
        else:
            print("Uso: python script.py [--file caminho] [--folder pasta]")
    else:
        modo_interativo()