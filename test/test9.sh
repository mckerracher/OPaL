#!/bin/bash

export LD_LIBRARY_PATH=build/
if build/marc; then
	echo Test 9 failed.
	exit 1
else
	exit 0
fi
