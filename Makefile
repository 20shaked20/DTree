.PHONY: all
all: ExampleOne ExampleTwo tree

ExampleOne: ftw1.c
		gcc ftw1.c -L. -o ExampleOne

ExampleTwo: ftw2.c
		gcc ftw2.c -L. -o ExampleTwo

#coder: codec.h our_codec.cpp our_codec.hpp
#	g++ our_codec.cpp ./libCodec.so -L. -l Codec -o coder -lpthread -ldl

.PHONY: clean
clean:
	-rm encoder ExampleOne ExampleTwo 2>/dev/null