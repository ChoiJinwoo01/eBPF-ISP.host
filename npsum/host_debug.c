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

//iteration
#define IT_TIME		1

#define MATRIX_SIZE 256*1024
#define MATRIX_ELEMENTS MATRIX_SIZE/4
#define NUM_MATRIX  1024

int *buf;

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);

	struct timeval tv;
	double start, end;
	double start_read, end_read, read_time;
	double start_comp, end_comp, comp_time;
	double read_acc, comp_acc, time_acc;

	int cnt = 0;
	buf = (int *)malloc(MATRIX_SIZE);

	read_acc = 0;
	comp_acc = 0;
	for(int t=0;t<IT_TIME;t++){
		gettimeofday(&tv, NULL);
		read_time = 0;
		comp_time = 0;
		start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		for(int i=0;i< NUM_MATRIX;i++){
			int sum = 0;
			struct nvme_user_io io = {
				.opcode     = 0x02,
				.flags      = 0,
				.control    = 0,
				.nblocks    = 64 -1,
				.rsvd       = 0,
				.metadata   = 0,
				.addr       = (__u64)(uintptr_t) buf,
				.slba       = 1024+i*64,
				.dsmgmt     = 0,
				.reftag     = 0,
				.appmask    = 0,
				.apptag     = 0,
			};
			gettimeofday(&tv, NULL);
			start_read = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			int err = ioctl(dev, NVME_IOCTL_SUBMIT_IO, &io);
			gettimeofday(&tv, NULL);
			end_read = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			read_time += end_read - start_read;
			if(err != 0){
				printf("iteration %d: err %d\n", i, err);
				continue;
			}
			gettimeofday(&tv, NULL);
			start_comp = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			for(int i=0;i<MATRIX_ELEMENTS;i++){
				sum += buf[i];
			}
			gettimeofday(&tv, NULL);
			end_comp = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			comp_time += end_comp - start_comp;
			//printf("sum : %d\n", sum);
			if(sum < 33000000 || sum > 34000000){
				printf("Warning: sum out of range: %d\n", sum);
			}
			printf("%d\n", sum);
		}
		gettimeofday(&tv, NULL);
		end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		//printf("host read time : %.2f ms\n", read_time / 1000);
		//printf("host comp time : %.2f ms\n", comp_time / 1000);
		time_acc += (end-start);
		read_acc += read_time;
		comp_acc += comp_time;
	}

	//printf("---- Test end ----\n");
	//printf("Average time : %.2f ms\n", time_acc / 1000 / IT_TIME);
	//printf("Average read time : %.2f ms\n", read_acc / 1000 / IT_TIME);
	//printf("Average comp time : %.2f ms\n", comp_acc / 1000 / IT_TIME);
	return 0;
}
