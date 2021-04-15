printf "CLI test for valid case: 2 arguments & 1 valid flag provided.\n";

export LD_LIBRARY_PATH=build/
build/marc --debug --output=output/test2.opl input/test2.opl
if [[ $? -ne 0 ]] ; then
  exit 1
fi
exit 0