#!/bin/bash

export LD_LIBRARY_PATH=build/
if build/marc --debug output=tmp.bin input/test2.opl  StarTrek>StarWars; then
	echo Test 14 failed.
	exit 1
else
	exit 0
fi