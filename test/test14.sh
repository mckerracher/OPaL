printf "build/marc --debug --output=test.tmp input/test2.opl invalid_argument\n";

export LD_LIBRARY_PATH=build/
build/marc --debug --output=test.tmp input/test2.opl invalid_argument
if [[ $? -ne 64 ]] ; then
  exit 1
fi
exit 0