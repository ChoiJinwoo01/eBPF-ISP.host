#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
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

//data: 32MB
#define data_blk	8*1024
#define data_size	4096*data_blk

//read
#define one_read_blk	64
#define one_read_size	4096*one_read_blk

#define MAX_TARGET_LEN 16

//iteration
#define IT	16

void *data;
char *read_ptr;

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR|O_DIRECT|O_SYNC);

	posix_memalign(&data, 4096, data_size);	
	read_ptr = data;
	memset(data, 0, data_size);

	struct timeval tv;
	double io_time, comp_time = 0;
	double io_start, io_end, io_acc = 0;
	double comp_start, comp_end, comp_acc = 0;

	char target_data[MAX_TARGET_LEN] = "Lorem";
	int target_len = strlen(target_data);
	int cnt = 0;

	//gettimeofday(&tv, NULL);
	for(int t=0;t<IT;t++){
		io_time = 0;
		comp_time = 0;
		cnt = 0;
		for(int i=0;i< (data_blk) / (one_read_blk);i++){
			gettimeofday(&tv, NULL);
			io_start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			int err = pread(dev, data, one_read_size,(1024+i*one_read_blk)*4096);
			gettimeofday(&tv, NULL);
			io_end= (tv.tv_sec) * 1000000 + (tv.tv_usec);
			io_time += io_end - io_start;
			if(err != one_read_size){
				printf("iteration %d: err %d\n", i, err);
				printf("ERROR: %s\n", strerror(errno));
				return 0;	
			}

			gettimeofday(&tv, NULL);
			comp_start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			for(int i=0;i<one_read_size - target_len+1;i++){
				if(read_ptr[i] == target_data[0]){
					if(strncmp(read_ptr+i, target_data, target_len) == 0){
						cnt++;
					}
				}
			}
			gettimeofday(&tv, NULL);
			comp_end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			comp_time += comp_end - comp_start;
		}
		printf("Result: %d\n", cnt);
		printf("I/O time : %.2f ms\n", io_time/1000);
		printf("Computation time : %.2f ms\n", comp_time/1000);
		io_acc += io_time;
		comp_acc += comp_time;
	}
	return 0;
}
