#!/bin/bash

export LD_LIBRARY_PATH=build/
if build/marc output=tmp.bin input/test2.opl; then
	exit 0
else
	echo Test 13 failed.
	exit 1
fi