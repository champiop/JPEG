CC = gcc
LD = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O0 -g

SRC_FILES=$(wildcard src/*.c)
OBJ_FILES=$(patsubst src/%.c,obj/%.o,$(SRC_FILES))

all: ppm2jpeg

ppm2jpeg: obj $(OBJ_FILES)
	$(LD) $(OBJ_FILES) -o $@

obj:
	mkdir obj -p

out:
	mkdir out -p
	mkdir out/logs -p

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf ppm2jpeg obj out

build: ppm2jpeg

run: out ppm2jpeg
	./ppm2jpeg > out/logs/out.log
