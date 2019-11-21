#!/bin/sh

# shell script to download files

CHECK=10932100
jobs=46

SIZE=$(du . | cut -f1)
echo $SIZE

mkdir inflated
for cnt in $(seq 0 $jobs)
do
  echo"$cnt"
  file="${cnt}_1.txt"
  parallel -j 50 :::: $file
  file="${cnt}_2.txt"
  parallel -j 50 :::: $file
  file="${cnt}_3.txt"
  parallel -j 50 :::: $file
  file="${cnt}_indexed.txt"
  cp $file inflated/
  rm *.ctx
  echo"${cnt} is done!"
  cnt=`expr $cnt + 1`
done



