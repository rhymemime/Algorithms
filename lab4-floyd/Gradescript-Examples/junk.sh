for i in 062 \
         063 \
         043 \
         065 \
         068 \
         070 ; do
  rm $i.txt
  while [ ! -f $i.txt ]; do
    sleep 1
    a.out > $i.txt
    j=`../NegativeGraphDiv1 < $i.txt`
    echo $i $j
    if [ $j -gt 500000 ]; then
      rm $i.txt
    fi
  done
done
