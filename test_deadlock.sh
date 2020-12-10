#!/bin/bash

for i in $(eval echo {1..$1})
do
    ./flip > /dev/null 2>&1
    echo ${i}
done
