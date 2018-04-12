#!/bin/bash

max=-999999999
min=99999999
sum=0
count=0

for i in $(cat number)
do
    if [ $i -le $min ]
    then
        min=$i
    fi
    if [ $i -ge $max ]
    then
        max=$i
    fi
    sum=$[sum+$i]
    count=$[$count + 1]
done

echo "Max: " $max
echo "Min: " $min
echo "Avg: " $(bc <<< "scale=4;$sum / $count")
