printf "build/marc --output=test.tmp input/test18.opl\n";

export LD_LIBRARY_PATH=build/
build/alex --output=test.tmp input/test18.opl
if [[ $? -ne 1 ]] ; then
  echo "Unsupported character found."
fi
exit 0
     
