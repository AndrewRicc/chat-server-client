# Compilatore e flag
CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread

# Directory
BUILD_DIR = build
SRC_DIR = src

# File di input
CHAT_CLIENT_SRC = $(SRC_DIR)/ChatClient.cpp
CHAT_CLIENT_OBJ = $(BUILD_DIR)/ChatClient.o

# Colori per l'output
GREEN = \033[0;32m
NC = \033[0m  # No Color

# Target principale
all: ask_nickname

# Crea la directory build se non esiste
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compila ChatClient.cpp
$(CHAT_CLIENT_OBJ): $(CHAT_CLIENT_SRC) | $(BUILD_DIR)
	@echo "$(GREEN)Compilando ChatClient...$(NC)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Target interattivo che chiede il nickname
ask_nickname:
	@read -p "Inserisci il tuo nickname: " nickname; \
	echo "$(GREEN)Generando main con nickname: $$nickname$(NC)"; \
	sed "s/NICKNAME_PLACEHOLDER/$$nickname/" $(SRC_DIR)/chat_client_template.cpp > $(SRC_DIR)/main_$$nickname.cpp; \
	$(MAKE) build_client NICKNAME=$$nickname

# Compila il client specifico
build_client: $(CHAT_CLIENT_OBJ)
	@echo "$(GREEN)Compilando il client per $(NICKNAME)...$(NC)"
	@$(CXX) $(CXXFLAGS) $(SRC_DIR)/main_$(NICKNAME).cpp $(CHAT_CLIENT_OBJ) -o $(BUILD_DIR)/chat_client_$(NICKNAME)
	@echo "$(GREEN)Client compilato con successo!$(NC)"
	@echo "Puoi avviare il client con: ./$(BUILD_DIR)/chat_client_$(NICKNAME)"

# Pulisci i file compilati
clean:
	@echo "$(GREEN)Pulizia dei file compilati...$(NC)"
	@rm -rf $(BUILD_DIR)
	@rm -f $(SRC_DIR)/main_*.cpp
	@echo "$(GREEN)Pulizia completata$(NC)"

.PHONY: all ask_nickname build_client clean