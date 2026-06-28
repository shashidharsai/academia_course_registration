CC = gcc
CFLAGS = -Wall

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

CLIENT = client

all: directories $(CLIENT)

directories:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(CLIENT): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(OBJ_DIR) $(CLIENT)

.PHONY: all clean directories