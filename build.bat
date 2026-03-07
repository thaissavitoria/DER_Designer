@echo off
echo Configurando projeto Qt com CMake para Visual Studio...

set CI_MODE=0
if /I "%GITHUB_ACTIONS%"=="true" set CI_MODE=1

REM 
if not exist "build" mkdir build
cd build

REM 
cmake .. -G "Visual Studio 17 2022" -A x64

REM 
if %ERRORLEVEL% EQU 0 (
    if %CI_MODE% EQU 1 (
        echo.
        echo Executando build Release para CI...
        cmake --build . --config Release --parallel

        if %ERRORLEVEL% NEQ 0 (
            echo.
            echo ====================================
            echo ERRO na compilacao Release!
            echo ====================================
            exit /b 1
        )
    )

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

if %CI_MODE% EQU 0 pause