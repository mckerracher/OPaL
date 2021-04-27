printf "build/alex --output=output/test18.lex input/test18.opl\n";

export LD_LIBRARY_PATH=build/
build/alex --output=output/test18.lex input/test18.opl
if [[ $? -ne 0 ]] ; then
  echo "Unsupported character found."
fi
exit 0
     
