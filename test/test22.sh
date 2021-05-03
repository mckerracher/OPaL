printf "build/astro --output=test22.ast input/test21.opl\n";
printf "diff -s output/test22.ast test/test22.ast";

export LD_LIBRARY_PATH=build/
build/astro --output=output/test22.ast input/test21.opl
diff -s output/test22.ast test/test22.ast
if [[ $? -ne 1 ]] ; then
  exit 1
fi
exit 0