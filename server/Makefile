CC = gcc
CFLAGS = -Wall -pthread

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
DATA_DIR = data

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

SERVER = server

all: directories $(SERVER)

directories:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(DATA_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(SERVER): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJ_DIR) $(SERVER)

.PHONY: all clean directories