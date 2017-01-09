CC=gcc
CFLAGS=-g -Wall -std=c99
LDFLAGS=
EXEC=bin/memory_access

SRC= $(wildcard src/*.c)
OBJ= $(SRC:.c=.o)

INC= 

$(EXEC): $(OBJ)
	@$(CC) -o $@ $^

src/%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

.PHONY:

clean:
	@rm -rf ./$(OBJ)

mrproper: clean
	@rm -rf $(EXEC)