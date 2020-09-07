EXEC = game
OBJECTS = main.o pacman.o ghost.o
CC = gcc

$(EXEC): $(OBJECTS)
	$(CC) $^ -o $@ -lpthread -lncurses -fsanitize=thread -ggdb

main.o: main.c main.h
	$(CC) -c -g main.c 

pacman.o: pacman.c main.h
	$(CC) -c -g pacman.c

ghost.o: ghost.c main.h
	$(CC) -c -g ghost.c

clean:
	rm -f $(OBJECTS) $(EXEC)

run:
	./$(EXEC) ${LIVES} ${GHOSTS}
