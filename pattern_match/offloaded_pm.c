#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/nvme_ioctl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <wait.h>
#include <linux/types.h>
#include <time.h>
#include <sys/time.h>

//data: 32MB
#define data_blk        8*1024
#define data_size       4096*data_blk

//read: MAX 256KB
#define one_read_blk    1
#define one_read_size   4096*one_read_blk

char data[one_read_size];

int callprog(int dev,int slba, int nlb, int param[]){
	int read_fd = open("./read.txt", O_CREAT|O_RDWR);
	char *data;
	struct timeval tv;
	double start, end;
	struct nvme_passthru_cmd cmd = {	//64bit
		.opcode = 0x82,
		.flags = 0,
		.rsvd1=0,
		.nsid=1,
		.cdw2=param[0],
		.cdw3=param[1],
		.metadata=0,
		.addr=0,
		.metadata_len=0,
		.data_len=nlb*4096,
		.cdw10=slba,		//slba
		.cdw11=0,		//slba(upper part of 64bit)
		.cdw12=nlb - 1,		//nlb
		.cdw13=0,
		.cdw14=0,
		.cdw15=0,
		.timeout_ms=0,
		.result=-1,
	};
	int err = ioctl(dev, NVME_IOCTL_IO_CMD, &cmd);
	if (err != 0){
		printf("ioctl issued err %d\n", err);
	}
	//printf("result is %d!\n", cmd.result);
	return cmd.result;
}

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);

	struct timeval tv;
        double start, end;
	int cnt=0;

	int slba = 1024;
	int param[5] = {0, 0, 0, 0, 0};
	

	gettimeofday(&tv, NULL);
	start = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	for(int i=0;i<(data_blk) / (one_read_blk);i++){
		cnt += callprog(dev, slba, one_read_blk, param);
		slba += one_read_blk;
	}
	gettimeofday(&tv, NULL);
	end = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	printf("ioctl time : %f\n", (end-start) / 1000);
	printf("result: %d\n", cnt);
}
