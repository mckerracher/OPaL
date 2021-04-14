#!/bin/bash

export LD_LIBRARY_PATH=build/
build/marc --debug --output=test.tmp
if [[ $? -ne 2 ]] ; then
  exit 1
fi
exit 0