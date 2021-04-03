CC := gcc
CFLAGS := -g -O0 -Wall -L./build -Wl,-rpath=./
LD_LIBRARY_PATH := build:$(LD_LIBRARY_PATH)
SHELL := env LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) /bin/bash

# Add an auto-incrementing build number global variable
BUILD_NUMBER_FILE=build.ver
BUILD_NUMBER_FLAGS = -D__BUILD_DATE=$$(date +'%Y%m%d')
BUILD_NUMBER_FLAGS += -D__BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))

all: dirs libopal marc tar $(BUILD_NUMBER_FILE)

# Increment build
$(BUILD_NUMBER_FILE): libopal marc
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 1 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

# Create required directory structure
dirs:
	mkdir -pv build tmp log report doc

# Build OPaL library
libopal: src/opal.c include/opal.h
	$(CC) $(BUILD_NUMBER_FLAGS) -g -O0 -fPIC -c -Wall src/opal.c -o build/opal.o
	ld -shared build/opal.o -o build/libopal.so

# Build MARC preprocessor
marc: libopal src/marc.c
	$(CC) $(CFLAGS) $(BUILD_NUMBER_FLAGS) src/marc.c -lopal -o build/marc

# Tar all files for release
tar: libopal marc
	tar -cvf build/opal_$$(cat $(BUILD_NUMBER_FILE))_$$(date +'%Y%m%d').tar build/libopal.so build/opal.o build/marc

.PHONY: test
test: clean all
	@printf "\n=== Test 1 - test1.opl - TODO ===\n"
	build/marc input/test1.opl output/test1.opl
	diff -s test/test1.opl test/test1.opl
	
	@printf "\n=== Test 2 - test2.opl - TODO ===\n"
	build/marc input/test2.opl output/test2.opl
	diff -s test/test2.opl test/test2.opl
	

.PHONY: clean
clean:
	# Delete binaries
	rm -fv build/*

	