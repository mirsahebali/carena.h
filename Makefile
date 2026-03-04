.PHONY: all expand tests

expand:
	gcc tests.c -E -P -o tests.i 

all:
	mkdir -p build
	gcc tests.c -ggdb -Wall -Wextra -fsanitize=undefined,address -o build/tests
	./tests
