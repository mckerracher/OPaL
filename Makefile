CC := gcc
CFLAGS := -g -O0 -Wall -L./build -Wl,-rpath=./ -D__BUILD_NUMBER=$$(date +'%s')
LD_LIBRARY_PATH := build:$(LD_LIBRARY_PATH)
SHELL := env LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) /bin/bash

all: dirs libopal marc tar

# Create required directory structure
dirs:
	mkdir -pv build tmp log report doc

# Build OPaL library
libopal: src/opal.c include/opal.h
	$(CC) -g -O0 -fPIC -c -Wall src/opal.c -o build/opal.o
	ld -shared build/opal.o -o build/libopal.so

# Build MARC preprocessor
marc: libopal src/marc.c
	$(CC) $(CFLAGS) src/marc.c -lopal -o build/marc

# Tar all files for release
tar: libopal marc
	tar -cvf build/opal_$$(date +'%s').tar build/libopal.so build/opal.o build/marc

.PHONY: test
test: clean all
	@printf "\n=== Test 1 - test1.opl - TODO ===\n"
	build/marc --debug --output=output/test1.opl input/test1.opl
	diff -s test/test1.opl test/test1.opl
	
	@printf "\n=== Test 2 - test2.opl - TODO ===\n"
	build/marc --debug --output=output/test2.opl input/test2.opl
	diff -s test/test2.opl test/test2.opl
	

.PHONY: clean
clean:
	# Delete binaries
	rm -fv build/*

	