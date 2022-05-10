#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PGSIZE 4096

int main(){
	int *data = (int *)malloc(PGSIZE);
	for(int i=0;i<PGSIZE/sizeof(int);i++){
		data[i] = i;
	}
	int fd = open("data.txt", O_CREAT|O_RDWR);
	write(fd, data, 4096);
	return 0;
}
