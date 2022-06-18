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



int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);

	struct timeval tv;
	double start, end;
	double time_acc;

	int cnt = 0;

	ioctl(dev, NVME_IOCTL_IO_CMD, &cmd_stats_clear);
	for(int t=0;t<IT_TIME;t++){
		//printf("Iteration %d\n", t);
		gettimeofday(&tv, NULL);
		start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		for(int i=0;i< NUM_MATRIX;i++){
			int sum = 0;
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
				.cdw10=1024+i*64,        //slba
				.cdw11=0,       //slba(upper part of 64bit)
				.cdw12=32 - 1,     //nlb
				.cdw13=0,
				.cdw14=0,
				.cdw15=0,
				.timeout_ms=0,
				.result=-1,
			};

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
		gettimeofday(&tv, NULL);
		end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		//printf("ioctl time : %.2f ms\n", (end-start) / 1000);
		time_acc += (end-start);
		ioctl(dev, NVME_IOCTL_IO_CMD, &cmd_stats);
		ioctl(dev, NVME_IOCTL_IO_CMD, &cmd_stats_clear);
	}

	printf("---- Test end ----\n");
	printf("Average time : %.2f ms\n", time_acc / 1000 / IT_TIME);
	return 0;
}
