CC := gcc
CFLAGS := -g -O0 -Wall -L./build -Wl,-rpath=./
LD_LIBRARY_PATH := build:$(LD_LIBRARY_PATH)
SHELL := env LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) /bin/bash

all: dirs libopal marc alex tar

# Create required directory structure
dirs:
	mkdir -pv build tmp log report doc output

# Build OPaL library
libopal: src/opal.c include/opal.h
	$(CC) -g -O0 -fPIC -c -Wall src/opal.c -o build/opal.o
	ld -shared build/opal.o -o build/libopal.so

# Build MARC preprocessor
marc: libopal src/marc.c
	$(CC) $(CFLAGS) src/marc.c -lopal -o build/marc

# Build ALEX preprocessor
alex: libopal src/alex.c
	$(CC) $(CFLAGS) src/alex.c -lopal -o build/alex

# Tar all files for release
tar: libopal marc alex
	tar -cvf build/opal.tar build/libopal.so build/opal.o build/marc build/alex

.PHONY: test
test: clean all
	@printf "\n=== Test 1 ===\n"
	build/marc --debug --output=output/test1.opl input/test1.opl
	diff -s output/test1.opl test/test1.opl
	
	@printf "\n=== Test 2 ===\n"
	build/marc --debug --output=output/test2.opl input/test2.opl
	diff -s output/test2.opl test/test2.opl
	
	@printf "\n=== Test 3 ===\n"
	build/marc --debug --output=output/test3.opl input/test3.opl
	diff -s output/test3.opl test/test3.opl
	
	@printf "\n=== Test 4 ===\n"
	build/marc --debug --output=output/test4.opl input/test4.opl
	diff -s output/test4.opl test/test4.opl
		
	@printf "\n=== Test 5 ===\n"
	build/marc --debug --output=output/test5.opl input/test5.opl
	diff -s output/test5.opl test/test5.opl
	
	@printf "\n=== Test 6 ===\n"
	build/marc --debug --output=output/test6.opl input/test6.opl
	diff -s output/test6.opl test/test6.opl
	
	@#printf "\n=== Test 7 ===\n"
	@#bash test/test7.sh || true
	
	@#printf "\n=== Test 8 ===\n"
	@#chmod -v 000 input/_RESTRICTED_.hpl
	@#bash test/test8.sh
	@#chmod -v 644 input/_RESTRICTED_.hpl
	
.PHONY: clean
clean:
	# Delete binaries
	rm -fv build/* output/* tmp/*

	