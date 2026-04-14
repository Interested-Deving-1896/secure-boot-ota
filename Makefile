
CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude

SRC=$(wildcard src/*.c)
OUT=fw

all:
	$(CC) $(CFLAGS) $(SRC) -lpthread -o $(OUT)

clean:
	rm -f $(OUT)
