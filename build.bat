@echo off
echo Configurando projeto Qt com CMake para Visual Studio...

REM Criar diretório de build se não existir
if not exist "build" mkdir build
cd build

REM Configurar com CMake para Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64

REM Verificar se a configuração foi bem-sucedida
if %ERRORLEVEL% EQU 0 (
    echo.
    echo ====================================
    echo Configuração concluída com sucesso!
    echo ====================================
    echo.
    echo Para compilar o projeto:
    echo   1. Abra o arquivo DERDesigner.sln no Visual Studio
    echo   2. Ou execute: cmake --build . --config Debug
    echo.
    echo Arquivos gerados em: build/
    echo.
) else (
    echo.
    echo ====================================
    echo ERRO na configuração!
    echo ====================================
    echo.
    echo Verifique se:
    echo   1. Qt6 está instalado e no PATH
    echo   2. Visual Studio 2022 está instalado
    echo   3. CMake está instalado e no PATH
    echo.
)

pause