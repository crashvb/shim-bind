#!/usr/bin/make -f

default: build

build: shim_bind.so test.out test

# Note: https://stackoverflow.com/a/13046079
shim_bind.so: shim_bind.c
	gcc -Wall -O2 -fpic -shared -o shim_bind.so shim_bind.c -ldl

test.out: test.c
	gcc -Wall -O2 -o test.out test.c

test: shim_bind.so test.out
	LD_PRELOAD=./shim_bind.so ./test.out

clean:
	rm --force shim_bind.so test.out
