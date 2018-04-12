#!/bin/bash

for i in I am oldboy teacher welcome to oldboy training class
do
    if test ${#i} -le 6
    then
        echo $i
    fi
done
