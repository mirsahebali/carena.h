.PHONY: tests expand tests

tests: check
	./build/tests

check:
	mkdir -p build
	gcc tests.c -std=c99 -ggdb -Wall -Wextra -fsanitize=undefined,address -o build/tests

expand:
	gcc tests.c -E -P -o tests.i 

