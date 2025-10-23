CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.
TARGET_PARENT = parent
TARGET_CHILD = child
SRC_DIR = src

all: $(TARGET_PARENT) $(TARGET_CHILD)

$(TARGET_PARENT): $(SRC_DIR)/parent.c $(SRC_DIR)/process.h $(SRC_DIR)/process.c
	$(CC) $(CFLAGS) -o $(TARGET_PARENT) $(SRC_DIR)/parent.c $(SRC_DIR)/process.c

$(TARGET_CHILD): $(SRC_DIR)/child.c $(SRC_DIR)/process.h $(SRC_DIR)/process.c
	$(CC) $(CFLAGS) -o $(TARGET_CHILD) $(SRC_DIR)/child.c $(SRC_DIR)/process.c

clean:
	rm -f $(TARGET_PARENT) $(TARGET_CHILD)

rebuild: clean all

.PHONY: all clean rebuild