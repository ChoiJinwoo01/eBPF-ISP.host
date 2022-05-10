#!/bin/bash

num=0
blk=0

while [ $num -le 16384 ]
do
	blk=$[ 1024+$num*64 ]
	sudo nvme write /dev/nvme0n1 -s $blk -c 63 -z 262144 -d data_256KB.txt
	((num++))
done
