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

//data: 32MB
#define data_blk	8*1024
#define data_size	4096*data_blk

//read: MAX 256KB(64 blks)
#define one_read_blk	64
#define one_read_size	4096*one_read_blk

#define MAX_TARGET_LEN 16

//iteration
#define IT	16

char data[one_read_size];

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);

	struct timeval tv;
	double start, end = 0;
	double total_acc = 0;
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
		gettimeofday(&tv, NULL);
		start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		for(int i=0;i< (data_blk) / (one_read_blk);i++){
			struct nvme_user_io io = {
				.opcode     = 0x02,
				.flags      = 0,
				.control    = 0,
				.nblocks    = one_read_blk-1,
				.rsvd       = 0,
				.metadata   = 0,
				.addr       = (__u64)(uintptr_t) data,
				.slba       = 1024+i*one_read_blk,
				.dsmgmt     = 0,
				.reftag     = 0,
				.appmask    = 0,
				.apptag     = 0,
			};
			gettimeofday(&tv, NULL);
			io_start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			int err = ioctl(dev, NVME_IOCTL_SUBMIT_IO, &io);
			gettimeofday(&tv, NULL);
			io_end= (tv.tv_sec) * 1000000 + (tv.tv_usec);
			io_time += io_end - io_start;
			if(err != 0){
				printf("iteration %d: err %d\n", i, err);
				continue;
			}
			gettimeofday(&tv, NULL);
			comp_start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			for(int i=0;i<one_read_size - target_len+1;i++){
				if(data[i] == target_data[0]){
					if(strncmp(data+i, target_data, target_len) == 0){
						cnt++;
					}
				}
			}
			gettimeofday(&tv, NULL);
			comp_end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
			comp_time += comp_end - comp_start;
		}
		gettimeofday(&tv, NULL);
		end = (tv.tv_sec)*1000000 + (tv.tv_usec);
		/*printf("Result: %d\n", cnt);
		printf("Overall time : %.2f ms\n", (end-start)/1000);
		printf("I/O time : %.2f ms\n", io_time/1000);
		printf("Computation time : %.2f ms\n", comp_time/1000);*/
		io_acc += io_time;
		comp_acc += comp_time;
		total_acc += (end-start)/1000;
	}
	printf("---- Test end ----\n");
	printf("Average time: %.2f ms\n", total_acc / IT);
	printf("Average I/O time: %.2f ms\n", io_acc / IT/1000);
	printf("Average Comptation time: %.2f ms\n", comp_acc / IT/1000);
	return 0;
}
