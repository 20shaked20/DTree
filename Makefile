.PHONY: all
all: SpecialTree

SpecialTree: stree.c
		gcc stree.c -L. -o stree

#coder: codec.h our_codec.cpp our_codec.hpp
#	g++ our_codec.cpp ./libCodec.so -L. -l Codec -o coder -lpthread -ldl

.PHONY: clean
clean:
	-rm encoder stree 2>/dev/null