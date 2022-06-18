#include <linux/bpf.h>
#include "bpf_helpers.h"

#define PGSIZE 4096

struct pblk_ctx{
	int param0;
	int param1;
	int param2;
	int param3;
	int param4;
	void *data;
	void *map_page;
};

SEC("pblk")
int pass(struct pblk_ctx *ctx){
	int cnt = 0;
	char *it = (char *) ctx->data;
	int i;
	for(i=0;i<PGSIZE*8;i++){
		if(it[i] != ' '){
			cnt++;
		}
	}
	return cnt;
}

char _license[] SEC("license") = "GPL";
