CC = gcc
LD = gcc

CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O0

SRC_FILES=$(wildcard src/*.c)
OBJ_FILES=$(patsubst src/%.c,obj/%.o,$(SRC_FILES))

all: ppm2jpeg

ppm2jpeg: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf ppm2jpeg $(OBJ_FILES) out/*.jpg out/logs/*.log

build: ppm2jpeg

run: ppm2jpeg
	./ppm2jpeg > out/logs/out.log
