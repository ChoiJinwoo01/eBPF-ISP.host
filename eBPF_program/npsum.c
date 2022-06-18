#include <linux/bpf.h>
#include "bpf_helpers.h"

#define AXIS0_LENGTH	64
#define AXIS1_LENGTH	1024
#define NUM_ELEMENTS	AXIS0_LENGTH*AXIS1_LENGTH

struct ebpf_ctx{
	unsigned int axis;
	int param1;
	int param2;
	int param3;
	int param4;	
	void *data;
	void *map_page;
};

SEC("nvmet")
int npsum(struct ebpf_ctx *ctx){
	int i, j = 0;
	int ret = 0;
	int axis = ctx->axis;
	int *array = ctx->data;
	int *result_array = ctx->map_page;
	if(axis == 0){
		for(i=0;i<AXIS0_LENGTH;i++){
			for(j=0;j<AXIS1_LENGTH;j++){
				result_array[j] += array[i*AXIS1_LENGTH + j];
			}
		}
	}
	else if (axis == 1){
		for(i=0;i<AXIS1_LENGTH;i++){
			for(j=0;j<AXIS0_LENGTH;j++){
				result_array[j] += array[i*AXIS0_LENGTH + j];
			}
		}
	}
	else{
		for(i=0;i<NUM_ELEMENTS;i++){
			ret += array[i];
		}
	}
	return ret;
}

char _license[] SEC("license") = "GPL";
