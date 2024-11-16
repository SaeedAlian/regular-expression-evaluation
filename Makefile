build:
	@g++ -o main.out src/main.c src/regex.c src/nfa.c src/util.c

debug:
	@g++ -g -o main.out src/main.c src/regex.c src/nfa.c src/util.c && gdb ./main.out

build-run: build
	@./main.out

run:
	@./main.out

valgrind: build
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main.out 

test-all:
	@g++ -o regex_test.out src/regex.c src/nfa.c test/regex_test.c && ./regex_test.out && rm ./regex_test.out
	@g++ -o string_test.out src/regex.c src/nfa.c test/string_test.c && ./string_test.out && rm ./string_test.out
