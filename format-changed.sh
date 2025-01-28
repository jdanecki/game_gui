#!/bin/bash

for f in `git diff --name-only -- '*.h' '*.cpp'`
do
    echo $f
    clang-format -i $f
done    

