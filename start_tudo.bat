@echo off
chcp 65001 > nul
echo.
echo 🔨 COMPILANDO ra2_main.cpp...
echo.

g++ -std=c++17 -O2 -o ra2_main.exe "core\ra2_main.cpp"

if %errorlevel% == 0 (
    echo ✅ SUCESSO!
    echo 🚀 EXECUTANDO...
    echo.
    ra2_main.exe
) else (
    echo ❌ ERRO NA COMPILAÇÃO!
    echo.
    echo 💡 Tente manualmente:
    echo g++ -std=c++17 -o ra2_main.exe core\ra2_main.cpp
)

pause