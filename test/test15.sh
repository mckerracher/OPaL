#!/bin/bash

export LD_LIBRARY_PATH=build/
build/marc --invalid_flag --output=test.tmp input/test2.opl invalid_argument
if [[ $? -ne 2 ]] ; then
  exit 1
fi
exit 0