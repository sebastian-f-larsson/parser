.PHONY: all example compile_flags

all:
	gcc -c src/parser.c -o lib/libparser.o -Wall -Wextra -pedantic -ggdb -O2 -Iinclude/ 
	ar rcs lib/libparser.a lib/libparser.o

example:
	gcc src/example.c -o example/example -Wall -Wextra -pedantic -ggdb -O2 -Iinclude/  -Llib/ -lparser
	gcc src/example_array.c -o example/example_array -Wall -Wextra -pedantic -ggdb -O2 -Iinclude/  -Llib/ -lparser

install: all
	sudo install -d /usr/include/parser
	sudo install -m 644 include/parser.h /usr/include/parser/parser.h
	sudo install -m 644 lib/libparser.a /usr/lib/libparser.a

uninstall:
	sudo rm -rf /usr/include/parser
	sudo rm -f /usr/lib/libparser

compile_flags:
	bear -- make
