CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

BIN_DIR = bin
DATA_DIR = data

TARGET_V1 = $(BIN_DIR)/v1_energy
TARGET_V2 = $(BIN_DIR)/v2_payload_sync
TARGET_V3 = $(BIN_DIR)/v3_full_makespan

TARGETS = $(TARGET_V1) $(TARGET_V2) $(TARGET_V3)

.PHONY: all clean run run-v1 run-v2 run-v3 run-medium run-large help

all: $(TARGETS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET_V1): v1_energy_model/main.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET_V2): v2_payload_sync/main.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(TARGET_V3): v3_full_makespan/main.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

v1: $(TARGET_V1)
v2: $(TARGET_V2)
v3: $(TARGET_V3)

run: $(TARGET_V3)
	@echo "=== Ejecutando Versión 3 (Makespan Completo) con dataset pequeño ==="
	./$(TARGET_V3) $(DATA_DIR)/instance_small_5.txt

run-v1: $(TARGET_V1)
	@echo "=== Ejecutando Versión 1 (Modelo de Energía) ==="
	./$(TARGET_V1) $(DATA_DIR)/instance_small_5.txt

run-v2: $(TARGET_V2)
	@echo "=== Ejecutando Versión 2 (Sincronización y Carga) ==="
	./$(TARGET_V2) $(DATA_DIR)/instance_small_5.txt

run-v3: $(TARGET_V3)
	@echo "=== Ejecutando Versión 3 (Makespan Completo) ==="
	./$(TARGET_V3) $(DATA_DIR)/instance_small_5.txt

run-medium: $(TARGET_V3)
	@echo "=== Ejecutando Versión 3 con dataset mediano (15 clientes) ==="
	./$(TARGET_V3) $(DATA_DIR)/instance_medium_15.txt

run-large: $(TARGET_V3)
	@echo "=== Ejecutando Versión 3 con dataset grande (30 clientes) ==="
	./$(TARGET_V3) $(DATA_DIR)/instance_large_30.txt

clean:
	rm -rf $(BIN_DIR)

help:
	@echo "Opciones disponibles:"
	@echo "  make            - Compila todas las versiones en bin/"
	@echo "  make v1         - Compila la Versión 1 (Modelo de Energía)"
	@echo "  make v2         - Compila la Versión 2 (Capacidad de Carga y Sincronización)"
	@echo "  make v3         - Compila la Versión 3 (Makespan y Estado)"
	@echo "  make run        - Ejecuta Versión 3 con dataset pequeño"
	@echo "  make run-medium - Ejecuta Versión 3 con dataset mediano"
	@echo "  make run-large  - Ejecuta Versión 3 con dataset grande"
	@echo "  make clean      - Elimina ejecutables generados"
