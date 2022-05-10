#include <linux/bpf.h>
#include "bpf_helpers.h"

struct bpf_map_def SEC("maps") rxcnt = {
        .type = BPF_MAP_TYPE_ARRAY,
        .key_size = 4,
        .value_size = 4,
        .max_entries = 256,
};

SEC("free-pass")
int pass(struct xdp_md *ctx){
	int i=0;
	long *value;

	value = bpf_map_lookup_elem(&rxcnt, &i);

	return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
