i=001
dir=/home/jplank/cs494/labs/Lab-6-RainbowGraph/Gradescript-Examples
while [ -f $dir/$i.txt ]; do
  time=`(time ./RG-Tester < $dir/$i.txt) 2>&1 | grep real | sed 's/m/ /' | awk '{ printf "%7.3f\n", $2*60+$3 }'`
  echo $time $dir/$i.txt
  i=`echo $i | awk '{ printf "%03d\n", $1+3 }'`
done

