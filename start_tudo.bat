@echo off
chcp 65001 > nul
echo.
echo COMPILANDO...
echo.

g++ -std=c++17 -O2 -o ra2_main.exe ra2_main.cpp

if %errorlevel% == 0 (
    echo SUCESSO!
    echo EXECUTANDO SISTEMA...
    echo.
    ra2_main.exe
) else (
    echo ERRO!

)

pause