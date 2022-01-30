CC = gcc
#CFLAGS = -Wall -Wextra -Wpedantic -std=c99
CFLAGS = -Wall -Wextra -ggdb
INPUT = main.c piece_table.c
OUTPUT = textedit

default:
	$(CC) $(CFLAGS) $(INPUT) -o $(OUTPUT)
