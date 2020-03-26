
while [ 1 ]; do
  read a b c d e
  n=`echo $a $b $c $d $e | awk '{ printf("%.0lf\n", $5 * $2 ) }'`
  e=`echo $n $c | awk '{ printf("%.0lf\n", $1*3*$2 ) }'`
  fn=`echo $a | awk '{ printf("%03d\n", $1) }'`
  seed=`echo $d | awk '{ printf "%.0lf\n", $1*100000 }'`
  echo $n $e $seed N > $fn.txt
  if [ $a = 50 ]; then
    exit 0
  fi
done
