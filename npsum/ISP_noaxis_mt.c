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
#define MATRIX_SIZE 256*1024
#define MATRIX_ELEMENTS MATRIX_SIZE/4
#define NUM_MATRIX  1024
#define IT_TIME		16

struct nvme_passthru_cmd cmd_stats ={  //64bit
	.opcode = 0x86,
	.flags = 0,
	.rsvd1=0,
	.nsid=1,
	.cdw2=0,
	.cdw3=0,
	.metadata=0,
	.addr       = 0,
	.metadata_len=0,
	.data_len=0,
	.cdw10=0,        //slba
	.cdw11=0,       //slba(upper part of 64bit)
	.cdw12=0,     //nlb
	.cdw13=0,
	.cdw14=0,
	.cdw15=0,
	.timeout_ms=0,
	.result=-1,
};

struct nvme_passthru_cmd cmd_stats_clear ={  //64bit
	.opcode = 0x87,
	.flags = 0,
	.rsvd1=0,
	.nsid=1,
	.cdw2=0,
	.cdw3=0,
	.metadata=0,
	.addr       = 0,
	.metadata_len=0,
	.data_len=0,
	.cdw10=0,        //slba
	.cdw11=0,       //slba(upper part of 64bit)
	.cdw12=0,     //nlb
	.cdw13=0,
	.cdw14=0,
	.cdw15=0,
	.timeout_ms=0,
	.result=-1,
};



int main(int argc, char *argv[]){
	int dev = open("/dev/nvme0n1", O_RDWR);

	struct timeval tv;
	double start, end;
	double time_acc=0;

	int num_threads = 1;
	if (argc > 1){
		num_threads = atoi(argv[1]);
	}
	else{
		printf("usage: ./a.out [num_threads]\n");
		return 0;
	}

	ioctl(dev, NVME_IOCTL_IO_CMD, &cmd_stats_clear);
	for(int it = 0; it< IT_TIME; it++){
		gettimeofday(&tv, NULL);
		start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		for(int t=0;t<num_threads;t++){
			if(fork() == 0){
				for(int i=0;i< NUM_MATRIX / num_threads;i++){
					int sum = 0;
					int target_slba = 1024 + i*64 + (NUM_MATRIX / num_threads) * 64 *t;
					struct nvme_passthru_cmd cmd = {    //64bit
						.opcode = 0x82,
						.flags = 0,
						.rsvd1=0,
						.nsid=1,
						.cdw2=3,	//sum with no axis
						.cdw3=0,
						.metadata=0,
						.addr       = 0,
						.metadata_len=0,
						.data_len=MATRIX_SIZE,
						.cdw10=target_slba,        //slba
						.cdw11=0,       //slba(upper part of 64bit)
						.cdw12=64 - 1,     //nlb
						.cdw13=0,
						.cdw14=0,
						.cdw15=0,
						.timeout_ms=0,
						.result=-1,
					};
					//	printf("slba %d\n", target_slba);

					int err = ioctl(dev, NVME_IOCTL_IO_CMD, &cmd);
					if(err != 0){
						printf("iteration %d: err %d\n", i, err);
						continue;
					}
					sum = cmd.result;
					if(sum < 30000000 || sum > 40000000){
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
		time_acc += (end-start);
		//printf("ioctl time : %.2f ms\n", (end-start) / 1000);
		//ioctl(dev, NVME_IOCTL_IO_CMD, &cmd_stats);
		//ioctl(dev, NVME_IOCTL_IO_CMD, &cmd_stats_clear);
		sleep(1);
	}

	printf("---- Test end ----\n");
	printf("Execution time : %.2f ms\n", time_acc / 1000 / IT_TIME);
	return 0;
}
