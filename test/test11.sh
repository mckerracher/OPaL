printf "build/marc --invalid_flag --output=output/test11.opl\n"

export LD_LIBRARY_PATH=build/
build/marc --invalid_flag --output=output/test11.opl
if [[ $? -ne 64 ]] ; then
  exit 1
fi
exit 0