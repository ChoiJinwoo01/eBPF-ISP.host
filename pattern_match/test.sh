#!/bin/bash

cnt=0
while (( "${cnt}" < 10 )); do
    echo "${cnt}"
    (( cnt = "${cnt}" + 1 ))
done
