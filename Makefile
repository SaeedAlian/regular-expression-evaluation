build:
	@g++ -o main.out src/main.c src/regex.c src/nfa.c

debug:
	@g++ -g -o main.out src/main.c src/regex.c src/nfa.c && gdb ./main.out

build-run: build
	@./main.out

run:
	@./main.out

valgrind: build
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main.out 

