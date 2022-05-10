#include <linux/bpf.h>
#include </mnt/yj/linux/tools/testing/selftests/bpf/bpf_helpers.h>

#define PGSIZE 4096

struct pblk_ctx{
	int param0;
	int param1;
	int param2;
	int param3;
	int param4;
	void *data;
};

SEC("pblk")
int pass(struct pblk_ctx *ctx){
	int cnt = 0;
	char *it = (char *) ctx->data;
	char target[8] = "Lorem";
	int i,j,t;
	for(t=0;t<10000;t++){
	for(i=0;i<32768-5;i++){
		for(j=0;j<5;j++){
			if(it[i+j] != target[j]){
				break;
			}
		}
		if(j == 5){
			cnt++;
		}
	}
	}
	return cnt;
}

char _license[] SEC("license") = "GPL";
