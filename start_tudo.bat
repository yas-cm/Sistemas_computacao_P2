@echo off
chcp 65001 > nul
echo.
echo 🔨 COMPILANDO PROJETO COMPLETO...
echo.

g++ -std=c++17 -O2 -o ra2_main.exe core/ra2_main.cpp

if %errorlevel% == 0 (
    echo ✅ SUCESSO!
    echo 🚀 EXECUTANDO...
    echo.
    ra2_main.exe
) else (
    echo ❌ ERRO!
    echo 💡 Verifique se os arquivos existem:
    echo   - core/ra2_main.cpp
    echo   - algorithms/cache_fifo.cpp  
    echo   - algorithms/cache_lru.cpp
)

pause