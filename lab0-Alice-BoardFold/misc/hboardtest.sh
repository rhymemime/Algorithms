#!/bin/sh

i=1
dir='/home/jplank/cs494/labs/Lab-0'

while [ $i -lt 51 ]; do

if [ $i -lt 10 ]; then
./boardFold $dir/Disney_Boards/00$i.cli < $dir/Disney_Boards/00$i.txt
else
./boardFold $dir/Disney_Boards/0$i.cli < $dir/Disney_Boards/0$i.txt
fi

i=`echo $i | awk '{ print $1 + 1 }'`
done
