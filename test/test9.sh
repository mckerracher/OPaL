#!/bin/bash

export LD_LIBRARY_PATH=build/
build/marc
if [[ $? -ne 64 ]] ; then
  exit 1
fi
exit 0
