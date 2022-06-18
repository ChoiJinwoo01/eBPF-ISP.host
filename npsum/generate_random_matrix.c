#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/nvme_ioctl.h>

#define MATRIX_SIZE	256*1024
#define	MATRIX_ELEMENTS	MATRIX_SIZE/4
#define NUM_MATRIX	1024
#define RANDOM_RANGE		1024

int *buf;

int main(){
	srand(time(NULL));
	buf = (int *)malloc(MATRIX_SIZE);

	int dev = open("/dev/nvme0n1", O_RDWR);

	for(int it=0;it<NUM_MATRIX;it++){
		int sum = 0;
		for(int i=0;i< MATRIX_ELEMENTS;i++){	//generate matrix and calculate sum
			buf[i] = rand() % RANDOM_RANGE;
			sum += buf[i];
		}

		char filename[64] = "./matrices/matrix";
		char number[8];
		char sum_str[16];
		sprintf(number, "%d", it);
		sprintf(sum_str, "%d", sum);
		strcat(filename, number);
		strcat(filename,".txt");
		//strcat(filename,"-");
		//strcat(filename, sum_str);
		int fd = open(filename, O_CREAT|O_RDWR);	//write matrix to file
		write(fd, buf, MATRIX_SIZE);
		close(fd);

		struct nvme_user_io io = {		//write matrix to SSD
			.opcode     = 0x01,
			.flags      = 0,
			.control    = 0,
			.nblocks    = 63,	// 64 Blocks(256KB)
			.rsvd       = 0,
			.metadata   = 0,
			.addr       = (__u64)(uintptr_t) buf,
			.slba       = 1024 + 64*it,
			.dsmgmt     = 0,
			.reftag     = 0,
			.appmask    = 0,
			.apptag     = 0,
		};
		int err = ioctl(dev, NVME_IOCTL_SUBMIT_IO, &io);
		if(err != 0){
			printf("error %d occured while writing matrix %d\n", err, it);
		}
	}
}
