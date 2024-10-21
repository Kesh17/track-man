main: src/main.c
	@mkdir -p bin
	clang src/main.c -o bin/main
