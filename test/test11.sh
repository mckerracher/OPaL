#!/bin/bash

export LD_LIBRARY_PATH=build/
if build/marc --gobbletygook output=test.bin; then
	echo Test 10 failed.
	exit 1
else
	exit 0
fi