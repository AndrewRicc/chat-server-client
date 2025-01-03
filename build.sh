#!/bin/bash

# Colori per l'output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}Iniziando il processo di build...${NC}"

# Crea la directory build se non esiste
if [ ! -d "build" ]; then
    echo -e "${GREEN}Creando la directory build...${NC}"
    mkdir build
fi

# Entra nella directory build
cd build

# Configura CMake
echo -e "${GREEN}Configurando CMake...${NC}"
if ! cmake ..; then
    echo -e "${RED}Errore nella configurazione di CMake${NC}"
    exit 1
fi

# Compila il progetto
echo -e "${GREEN}Compilando il progetto...${NC}"
if ! cmake --build .; then
    echo -e "${RED}Errore nella compilazione${NC}"
    exit 1
fi

echo -e "${GREEN}Build completata con successo!${NC}"
echo -e "Puoi trovare l'eseguibile in: ${GREEN}build/chat_client${NC}"

# Torna alla directory originale
cd ..