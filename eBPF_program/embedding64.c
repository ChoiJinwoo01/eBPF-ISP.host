#include <linux/bpf.h>
#include "bpf_helpers.h"

#include <linux/types.h>
#include <stdint.h>

#include <math.h>

#define PGSIZE	4096


struct pblk_ctx{
	int result_id;
	unsigned int embedding_id;
	unsigned int vector_length;
	int param3;
	int param4;
	void *data;
	void *map_page;
};

SEC("pblk")
int embedding(struct pblk_ctx *ctx){
	int i=0;
	int ret=4;
	unsigned int vector_length = 64;
	unsigned int attribute_size = 4;
	unsigned int off = (ctx->embedding_id % (4096 / (attribute_size * vector_length)));
	int *target_vector = ctx->data;
	target_vector += vector_length * off;
	int *map_ptr = ctx->map_page;
	map_ptr += vector_length * ctx->result_id;

	for(i=0;i<vector_length;i++){
		*map_ptr += *target_vector;
		target_vector++;
		map_ptr++;
	}
	return ret;
}

char _license[] SEC("license") = "GPL";
