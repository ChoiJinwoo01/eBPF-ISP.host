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

//data: 1GB
#define data_blk	256*1024
#define data_size	4096*data_blk

//read: MAX 512KB(128 blks)
#define one_read_blk	128
#define one_read_size	4096*one_read_blk

char data[one_read_size];

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR|O_SYNC);

	struct timeval tv;
	double start, end;
	double time_acc;

	char *data_str = (char *)data;
	int cnt = 0;

	//printf("Iteration %d\n", t);
	gettimeofday(&tv, NULL);
	start = (tv.tv_sec) * 1000000 + (tv.tv_usec);
	for(int i=0;i< ((data_blk) / (one_read_blk));i++){
		struct nvme_passthru_cmd cmd = {    //64bit
			.opcode = 0x82,
			.flags = 0,
			.rsvd1=0,
			.nsid=1,
			.cdw2=0,
			.cdw3=0,
			.metadata=0,
			.addr       = (__u64)(uintptr_t) data,
			.metadata_len=0,
			.data_len=one_read_blk * 4096,
			.cdw10=1024+i*one_read_blk,        //slba
			.cdw11=0,       //slba(upper part of 64bit)
			.cdw12=one_read_blk - 1,     //nlb
			.cdw13=0,
			.cdw14=0,
			.cdw15=0,
			.timeout_ms=0,
			.result=-1,
		};

		int err = ioctl(dev, NVME_IOCTL_IO_CMD, &cmd);
		if(err != 0){
			printf("iteration %d: err %d\n", i, err);
			return 0;
		}
		cnt += cmd.result;
	}
	gettimeofday(&tv, NULL);
	end = (tv.tv_sec) * 1000000 + (tv.tv_usec);
	printf("Result: %d\n", cnt);
	printf("1GB isp read time : %.2f ms\n", (end-start) / 1000);
	return 0;
}
