fio --bs=512k --numjobs=$1 --iodepth=1 --size=4G --loops=1--direct=1 --invalidate=1 --fsync_on_close=1 --randrepeat=1 --norandommap --filename=/dev/sda6 --name=read-phase --rw=$2 --ioengine=$3
