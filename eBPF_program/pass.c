#include <linux/bpf.h>
#include "bpf_helpers.h"

SEC("pass")
int pass(struct xdp_md *ctx){
	return 1;
}

char _license[] SEC("license") = "GPL";
