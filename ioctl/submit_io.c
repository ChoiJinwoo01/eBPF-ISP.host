#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/nvme_ioctl.h>
#include <sys/stat.h>
#include <stdint.h>

int main(){
	int dev = open("/dev/nvme0n1", O_RDWR);
	printf("dev is %d\n", dev);

	void *data;
	data = (void *)malloc(32768);

	/*struct nvme_passthru_cmd cmd = {
		.opcode = 0x02,
		.flags = 0,
		.rsvd1=0,
		.nsid=1,
		.cdw2=0,
		.cdw3=0,
		.metadata=0,
		.addr=(__u64)(uintptr_t) data,
		.metadata_len=0,
		.data_len=10000,
		.cdw10=100,
		.cdw11=0,
		.cdw12=0,
		.cdw13=0,
		.cdw14=0,
		.cdw15=0,
		.timeout_ms=0,
		.result=2,
	};*/

	struct nvme_user_io io = {
	  .opcode     = 0x02,
	  .flags      = 0,
	  .control    = 0,
	  .nblocks    = 7,
	  .rsvd       = 0,
	  .metadata   = 0,
	  .addr       = (__u64)(uintptr_t) data,
	  .slba       = 1024,
	  .dsmgmt     = 0,
	  .reftag     = 0,
	  .appmask    = 0,
	  .apptag     = 0,
	  };
	int err = 0;
	err = ioctl(dev, NVME_IOCTL_SUBMIT_IO, &io);
	printf("error code: %x\n", err);
}
