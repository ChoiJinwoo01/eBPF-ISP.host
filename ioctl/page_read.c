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
#include <string.h>

int callprog(int dev,int slba, int nlb, int param[]){
	int read_fd = open("./page.txt", O_CREAT|O_RDWR);
	char *data;
	struct timeval tv;
	double start, end;
	data = (char *)malloc(4096);
	memset(data, 0, 4096);
	int *intdata;
	struct nvme_passthru_cmd cmd = {	//64bit
		.opcode = 0x84,
		.flags = 0,
		.rsvd1=0,
		.nsid=1,
		.cdw2=param[0],
		.cdw3=param[1],
		.metadata=(__u64)param[2] <<32 | param[3],
		.addr=(__u64)(uintptr_t) data,
		.metadata_len=0,
		.data_len=4096*nlb,
		.cdw10=1024,		//slba
		.cdw11=0,		//slba(upper part of 64bit)
		.cdw12=0,		//nlb
		.cdw13=0,
		.cdw14=0,
		.cdw15=0,
		.timeout_ms=0,
		.result=123,
	};
	int ret = ioctl(dev, NVME_IOCTL_IO_CMD, &cmd);	//64bit
	write(read_fd,data,4096);
	intdata = (int *)data;
	for(int i=0;i<32;i++){
		printf("%d ",*intdata);
		if(i %8 == 7){
			printf("\n");
		}
		intdata++;

	}
	return ret;
}

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);

	int nlb = 1;
	int slba = 1024;
	int param[5] = {2, 8, 0, 0, 0};

	int err = callprog(dev, slba, nlb, param);

	printf("error code: %d\n", err);
}
