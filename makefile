all: programme
	./main 5

programme: main.c
	gcc -Wall main.c -o main -lm