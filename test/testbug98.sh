printf "build/alex --debug input/bug-98-1.opl\n";
build/alex --debug input/bug-98-1.opl;
if [[ $? -ne 1 ]] ; then
  exit 1
fi

printf "build/alex --debug input/bug-98-2.opl\n";
build/alex --debug input/bug-98-2.opl;
if [[ $? -ne 1 ]] ; then
  exit 1
fi

exit 0
