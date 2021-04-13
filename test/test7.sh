printf "build/marc --debug input/test7.opl\n";
build/marc --debug input/test7.opl;
if [[ $? -ne 2 ]] ; then
  exit 1
fi

exit 0
