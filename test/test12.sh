#!/bin/bash

export LD_LIBRARY_PATH=build/
build/marc --debug --output=test.tmp input/test2.opl
if [[ $? -ne 2 ]] ; then
  exit 1
fi
exit 0