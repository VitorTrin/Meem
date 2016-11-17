CC=gcc
DEPS=input.h
OBJ1=rm.o input.o
OBJ2=mkdir.o
OBJ3=touch.o

%.o: %.c $(DEPS)
	$(CC) $< -c -o $@

rm: $(OBJ1)
	$(CC) $^ -o $@

mkdir: $(OBJ2)
	$(CC) $^ -o $@

touch: $(OBJ3)
	$(CC) $^ -o $@