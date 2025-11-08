CC = g++
CFLAGS = -Wall -Iinclude -g -Iinclude

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SERVER_SRC = $(SRC_DIR)/server.cpp
SERVER_OBJ = $(OBJ_DIR)/server.o
SERVER_BIN = $(BIN_DIR)/server

CLIENT_SRC = $(SRC_DIR)/client.cpp
CLIENT_OBJ = $(OBJ_DIR)/client.o
CLIENT_BIN = $(BIN_DIR)/client


COMMON_SRC = $(SRC_DIR)/common.cpp
COMMON_OBJ = $(OBJ_DIR)/common.o
COMMON_BIN = $(BIN_DIR)/common


all: $(SERVER_BIN) $(CLIENT_BIN)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build object files
$(SERVER_OBJ): $(SERVER_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ): $(CLIENT_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(COMMON_OBJ): $(COMMON_SRC) | $(COMMON_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# Link executables
$(SERVER_BIN): $(SERVER_OBJ) $(COMMON_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_BIN): $(CLIENT_OBJ) $(COMMON_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@




# Convenience targets
server: $(SERVER_BIN)
client: $(CLIENT_BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean server client
