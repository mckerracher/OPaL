printf "build/marc --debug --output=output/test10.opl\n";

export LD_LIBRARY_PATH=build/
build/marc --debug --output=output/test10.opl
if [[ $? -ne 64 ]] ; then
  exit 1
fi
exit 0