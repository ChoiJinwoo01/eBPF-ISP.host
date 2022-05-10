#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/nvme_ioctl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <linux/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

//data: 1GB
#define data_blk	256*1024
#define data_size	4096*data_blk

//read: MAX 512KB(128 blks)
#define one_read_blk	128
#define one_read_size	4096*one_read_blk

#define loops		4

#define threads 	2

void *data;

int main(){
	for(int i=0;i<threads;i++){
		if(fork()==0){
			int dev = open("/dev/sda6", O_RDWR|O_DIRECT);
			posix_memalign(&data, 4096, data_size);
			memset(data, 0, data_size);

			struct timeval tv;
			double start, end;
			double time_acc;
			for(int i=0;i<loops;i++){
				gettimeofday(&tv, NULL);
				start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
				for(int i=0;i< ((data_blk) / (one_read_blk));i++){
					int off = (1024+i*one_read_blk)*4096;
					int err = pread(dev, data, one_read_size, off);
					if(err != one_read_size){
						printf("iteration %d: err %d, off 0x%x\n", i, err,off);
						printf("ERROR: %s\n", strerror(errno));
						continue;
					}
				}
				gettimeofday(&tv, NULL);
				end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
				printf("1GB pread time : %.2f ms\n", (end-start) / 1000);
				time_acc += (end-start);
			}
			printf("---- Test end ----\n");
			printf("Average time : %.2f ms\n", time_acc / 1000 / loops);
		}
		int status = 0;
		while(wait(&status)<0);	
		printf("---- All processes Finished ----\n");
	}
	return 0;
}
