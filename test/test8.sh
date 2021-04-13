printf "build/marc --debug input/test8.opl\n";
build/marc --debug input/test8.opl;
if [[ $? -ne 13 ]] ; then
  exit 1
fi

exit 0
