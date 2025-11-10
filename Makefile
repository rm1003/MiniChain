# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O0 -g
LDFLAGS = -lcryptopp

# Diretórios
SRC_DIR = src
INC = $(SRC_DIR)

# Fontes
SRV_SRC = $(SRC_DIR)/server/server.cpp
CLT_SRC = $(SRC_DIR)/client/client.cpp

# Bins
SRV_BIN = server
CLT_BIN = client

# Alvo padrão
all: $(SRV_BIN) $(CLT_BIN)

$(SRV_BIN): $(SRV_SRC)
	$(CXX) $(CXXFLAGS) -I$(INC) $^ -o $@ $(LDFLAGS)

$(CLT_BIN): $(CLT_SRC)
	$(CXX) $(CXXFLAGS) -I$(INC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(SRV_BIN) $(CLT_BIN)

.PHONY: all clean
