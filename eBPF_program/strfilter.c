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
	char target[16] = "Lorem";
	char target_len = 5;
	int i,j;
	for(i=0;i<PGSIZE*128-target_len;i++){
		if(it[i] == target[0]){
			for(j=0;j<target_len;j++){
				if(it[i+j] != target[j]){
					break;
				}
			}
			if(j == target_len){
				cnt++;
			}
		}
	}
	return cnt;
}

char _license[] SEC("license") = "GPL";
