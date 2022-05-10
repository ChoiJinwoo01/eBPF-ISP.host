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
#include <sys/errno.h>

//data: 32MB
#define data_blk	8*1024
#define data_size	4096*data_blk

#define MAX_TARGET_LEN 16

void *data;
char *read_ptr;

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);
	
	posix_memalign(&data, 4096, data_size);
	read_ptr = data;
	if(dev < 0){
		printf("dev : %d\n", dev);
		return 0;
	}

	struct timeval tv;
	double start, end = 0;
	double total_acc = 0;
	double io_time, comp_time = 0;
	double io_start, io_end, io_acc = 0;
	double comp_start, comp_end, comp_acc = 0;

	char target_data[MAX_TARGET_LEN] = "Lorem";
	int target_len = strlen(target_data);
	int cnt = 0;

	memset(data, 0, data_size);

	io_time = 0;
	comp_time = 0;
	cnt = 0;
	gettimeofday(&tv, NULL);
	start = (tv.tv_sec) * 1000000 + (tv.tv_usec);

	/* starts I/O */
	gettimeofday(&tv, NULL);
	io_start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
	pread(dev, data, data_size, 1024*4096);
	gettimeofday(&tv, NULL);
	io_end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
	io_time = io_end - io_start;

	/* starts Computation */
	gettimeofday(&tv, NULL);
	comp_start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
	for(int i=0;i<data_size - target_len+1;i++){
		if(read_ptr[i] == target_data[0]){
			if(strncmp(read_ptr+i, target_data, target_len) == 0){
				cnt++;
			}
		}
	}
	gettimeofday(&tv, NULL);
	comp_end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
	comp_time += comp_end - comp_start;
	gettimeofday(&tv, NULL);
	end = (tv.tv_sec)*1000000 + (tv.tv_usec);

	/* print results */
	printf("Result: %d\n", cnt);
	printf("I/O time : %.2f ms\n", io_time/1000);
	printf("Computation time : %.2f ms\n", comp_time/1000);
	io_acc += io_time;
	comp_acc += comp_time;
	total_acc += (end-start)/1000;
	/*printf("---- Test end ----\n");
	  printf("Average time: %.2f ms\n", total_acc / IT);
	  printf("Average I/O time: %.2f ms\n", io_acc / IT/1000);
	  printf("Average Comptation time: %.2f ms\n", comp_acc / IT/1000);*/
	return 0;
}
