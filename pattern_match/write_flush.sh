#!/bin/bash

num=0
blk=0

while [ $num -le 10240 ]
do
	blk=$[ 10240+$num*8 ]
	sudo nvme write /dev/nvme0n1 -s $blk -c 7 -z 32768 -d data.txt
	((num++))
done
