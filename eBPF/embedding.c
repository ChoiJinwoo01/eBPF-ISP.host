#include <linux/bpf.h>
//#include </mnt/yj/linux/tools/testing/selftests/bpf/bpf_helpers.h>
#include "bpf_helpers.h"

#define PGSIZE 4096

struct pblk_ctx{
	int offset;
	int embedding_length;
	int result_addr;
	int attsize;
	int param4;
	void *data;
};

struct bpf_map_def SEC("maps") rxcnt = {
	.type = BPF_MAP_TYPE_ARRAY,
	.key_size = 4,
	.value_size = sizeof(long),
	.max_entries = 256,
};

SEC("pblk")
int pass(struct pblk_ctx *ctx){
	int i=0;
	int ret=0;
	int *it = (int *)ctx->data;
	int target = ctx->offset * ctx->embedding_length;

	for(i=0;i<ctx->embedding_length;i++){
		ret += *it;
		it++;
	}
	return ret;
}

char _license[] SEC("license") = "GPL";
