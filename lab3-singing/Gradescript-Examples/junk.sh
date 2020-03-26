i=70
while [ $i -lt 101 ]; do
   f=`echo $i | awk '{ printf "%03d.txt", $1 }'`
   echo $f
   a.out 1000 > $f
   sleep 1
   i=`echo $i | awk '{ print $1+1 }'`
done
