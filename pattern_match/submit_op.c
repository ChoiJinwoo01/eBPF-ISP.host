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
		.metadata=(__u64)param[2] <<32 | param[3],
		.addr=0,
		.metadata_len=0,
		.data_len=0,
		.cdw10=1024,		//slba
		.cdw11=0,		//slba(upper part of 64bit)
		.cdw12=nlb - 1,		//nlb
		.cdw13=0,
		.cdw14=0,
		.cdw15=0,
		.timeout_ms=0,
		.result=123,
	};
	gettimeofday(&tv, NULL);
	start = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	int ret = ioctl(dev, NVME_IOCTL_IO_CMD, &cmd);	//64bit
	gettimeofday(&tv, NULL);
	end = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
	printf("ioctl time : %f\n", (end-start) / 1000);
	printf("eBPF result: %d\n", cmd.result);
	write(read_fd,data,4096*nlb);
	return ret;
}

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);

	int nlb = 1;
	int slba = 1024;
	int param[5] = {0, 1, 0, 0, 0};

	int err = callprog(dev, slba, nlb, param);

	printf("error code: %d\n", err);
}
