# Makefile - Simulador de Combate (Estrutura com pastas)
# Estrutura:
# cpp_endgame/
# ├── cpp_files/
# │   ├── simulador_I.cpp
# │   └── simulador_II.cpp
# ├── exe_files/
# │   └── (executáveis gerados aqui)
# ├── material/
# ├── README.md
# └── Makefile (este arquivo)

# Variáveis
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Caminhos
CPP_DIR = cpp_files
EXE_DIR = exe_files

# Executáveis com caminho completo
SIM_I = $(EXE_DIR)/simulador_I
SIM_II = $(EXE_DIR)/simulador_II

# Arquivos fonte
SRC_I = $(CPP_DIR)/simulador_I.cpp
SRC_II = $(CPP_DIR)/simulador_II.cpp

# Alvo padrão (executado com "make")
all: $(SIM_I) $(SIM_II)
	@echo "✅ Ambos os simuladores compilados!"

# Compilar Simulador I
$(SIM_I): $(SRC_I)
	@mkdir -p $(EXE_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<
	@echo "✅ Simulador I compilado: $@"

# Compilar Simulador II
$(SIM_II): $(SRC_II)
	@mkdir -p $(EXE_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<
	@echo "✅ Simulador II compilado: $@"

# Rodar Simulador I
run_I: $(SIM_I)
	@echo "🎮 Executando Simulador I..."
	@./$(SIM_I)

# Rodar Simulador II
run_II: $(SIM_II)
	@echo "🎮 Executando Simulador II..."
	@./$(SIM_II)

# Executar ambos (um após o outro)
run_all: $(SIM_I) $(SIM_II)
	@echo "🎮 Executando Simulador I..."
	@./$(SIM_I)
	@echo ""
	@echo "🎮 Executando Simulador II..."
	@./$(SIM_II)

# Limpar executáveis
clean:
	@rm -f $(SIM_I) $(SIM_II)
	@echo "🗑️  Executáveis removidos"

# Limpeza profunda (remove pasta exe_files se vazia)
clean_all: clean
	@rmdir $(EXE_DIR) 2>/dev/null || true
	@echo "🗑️  Pasta exe_files removida"

# Info
info:
	@echo "=== Informações do Projeto ==="
	@echo "Compilador: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Pasta fonte: $(CPP_DIR)"
	@echo "Pasta executáveis: $(EXE_DIR)"
	@echo ""
	@echo "=== Comandos Disponíveis ==="
	@echo "make              - Compila ambos"
	@echo "make run_I        - Compila e executa Simulador I"
	@echo "make run_II       - Compila e executa Simulador II"
	@echo "make run_all      - Compila e executa ambos"
	@echo "make clean        - Remove executáveis"
	@echo "make clean_all    - Remove executáveis e pasta vazia"
	@echo "make info         - Mostra esta mensagem"

# Declarar que esses não são arquivos reais
.PHONY: all run_I run_II run_all clean clean_all info
