printf "build/alex --output=tmp/test19.lex input/test19.opl\n";

export LD_LIBRARY_PATH=build/
build/alex --output=tmp/test19.lex input/test19.opl
if [[ $? -ne 0 ]] ; then
  echo "Unsupported character found."
fi
exit 0

~                  
