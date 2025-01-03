@echo off
setlocal

echo [92mIniziando il processo di build...[0m

:: Crea la directory build se non esiste
if not exist build (
    echo [92mCreando la directory build...[0m
    mkdir build
)

:: Entra nella directory build
cd build

:: Configura CMake
echo [92mConfigurando CMake...[0m
cmake .. -G "MinGW Makefiles"
if errorlevel 1 (
    echo [91mErrore nella configurazione di CMake[0m
    exit /b 1
)

:: Compila il progetto
echo [92mCompilando il progetto...[0m
cmake --build .
if errorlevel 1 (
    echo [91mErrore nella compilazione[0m
    exit /b 1
)

echo [92mBuild completata con successo![0m
echo Puoi trovare l'eseguibile in: [92mbuild\chat_client.exe[0m

:: Torna alla directory originale
cd ..