#!/bin/bash

a=1

for i in $(seq 20)
do
    a=$[a*$i]
done

echo "result" $a
