build:
	@g++ -o main.out src/main.c src/regex.c src/nfa.c

build-run: build
	@./main.out

run:
	@./main.out

