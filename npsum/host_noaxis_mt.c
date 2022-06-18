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

//iteration
#define IT_TIME		32

#define MATRIX_SIZE 256*1024
#define MATRIX_ELEMENTS MATRIX_SIZE/4
#define NUM_MATRIX  1024

int *buf;

void *buf_mem;

int main(int argc, char *argv[]){
	int dev = open("/dev/nvme0n1", O_RDWR|O_DIRECT);

	int num_threads = 1;
	if(argc > 1){
		num_threads = atoi(argv[1]);
	}
	else{
		printf("usage: ./a.out [num_threads]\n");
	}

	struct timeval tv;
	double start, end  = 0;
	double start_read, end_read, read_time = 0;
	double start_comp, end_comp, comp_time = 0;
	double read_acc, comp_acc, time_acc = 0;

	int cnt = 0;
	posix_memalign(&buf_mem, 4096,MATRIX_SIZE);
	buf = buf_mem;

	read_acc = 0;
	comp_acc = 0;
	for(int t=0;t<IT_TIME;t++){
		gettimeofday(&tv, NULL);
		start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		for(int t=0;t<num_threads;t++){
			if(fork() == 0){
				for(int i=0;i< NUM_MATRIX/num_threads;i++){
					int sum = 0;
					struct nvme_user_io io = {
						.opcode     = 0x02,
						.flags      = 0,
						.control    = 0,
						.nblocks    = 64 -1,
						.rsvd       = 0,
						.metadata   = 0,
						.addr       = (__u64)(uintptr_t) buf,
						.slba       = 1024+i*64+t*64*(NUM_MATRIX/num_threads),
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
				}
				exit(0);
			}
		}
		int status = 0;
		while(wait(&status) > 0);
		gettimeofday(&tv, NULL);
		end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		//printf("host read time : %.2f ms\n", read_time / 1000);
		//printf("host comp time : %.2f ms\n", comp_time / 1000);
		time_acc += (end-start);
		sleep(1);
	}

	printf("---- Test end ----\n");
	printf("Average time : %.2f ms\n", time_acc / 1000 / IT_TIME);
	return 0;
}
