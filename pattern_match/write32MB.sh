#!/bin/bash

num=0
blk=0

while [ $num -le 512 ]
do
	blk=$[ 1024+$num*16 ]
	sudo nvme write /dev/nvme0n1 -s $blk -c 7 -z 32768 -d data.txt
	((num++))
done
num=0
while [ $num -le 512 ]
do
	blk=$[ 1024+8+$num*16 ]
	sudo nvme write /dev/nvme0n1 -s $blk -c 7 -z 32768 -d data2.txt
	((num++))
done
