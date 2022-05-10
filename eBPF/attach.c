#include <linux/bpf.h>
//#include <bpf/libbpf.h>
#include </mnt/yj/linux/tools/lib/bpf/libbpf.h>
#include <net/if.h>
#include <linux/if_link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
	struct bpf_object *obj;
	char input_file[20] = "./";
	if(argc < 2){
		printf("Usage: ./a.out [prog]\n");
		return 0;
	}
	strcat(input_file, argv[1]);
	strcat(input_file, ".o");
	int prog_fd=123456;
	struct bpf_prog_load_attr prog_load_attr = {
		.prog_type = BPF_PROG_TYPE_XDP,
//		.prog_type = BPF_PROG_TYPE_PBLK,
		.file = input_file,
	};
	struct bpf_map *map;
	int err = bpf_prog_load_xattr(&prog_load_attr, &obj, &prog_fd);
	printf("prog_fd : %d\n", prog_fd);
	printf("err code: %d\n", err);

	map = bpf_map__next(NULL, obj);
	if (!map) {
		printf("There is no map in this obj file\n");
	}
	while(1){

	}

	return 0;
}
