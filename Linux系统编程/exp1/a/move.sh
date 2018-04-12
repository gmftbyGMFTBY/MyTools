#!/bin/bash

for i in $(ls *.c)
do
    mv $i ../b/
done

echo "Move finish!"

ls -l ../b/* | sort

