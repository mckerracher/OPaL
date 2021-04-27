printf "build/marc --debug input/test8.opl\n";

chmod -v 000 input/_RESTRICTED_.hpl
build/marc --debug input/test8.opl;
if [[ $? -ne 13 ]] ; then
  exit 1
fi

chmod -v 644 input/_RESTRICTED_.hpl
exit 0