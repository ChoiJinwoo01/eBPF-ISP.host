#include <linux/bpf.h>
#include </mnt/yj/linux/tools/testing/selftests/bpf/bpf_helpers.h>
#include <unistd.h>

#define PGSIZE 4096

struct pblk_ctx{
	int offset;
	int length;
	int result_addr;
	int attsize;
	int param4;
	void *data;
};

SEC("pblk")
int pass(struct pblk_ctx *ctx){
	int ret = 0;
	if(ctx->offset != 0){
		sleep(3);
	}
	else{
		ret = 1;
	}
	return ret;
}

char _license[] SEC("license") = "GPL";
