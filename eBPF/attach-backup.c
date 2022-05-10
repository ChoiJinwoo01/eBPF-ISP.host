#include <linux/bpf.h>
//#include "bpf/libbpf.h"
#include "/usr/src/linux-5.4.3+/tools/lib/bpf/libbpf.h"

int main(){
	struct bpf_object *obj;
	int fd;
	bpf_prog_load("./pass.o", BPF_PROG_TYPE_XDP,&obj ,&fd);
}
