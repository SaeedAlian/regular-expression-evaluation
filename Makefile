build:
	@g++ -o main.out main.c

build-run: build
	@./main.out

run:
	@./main.out

