#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int *buf;
int *result;

int main(){
	buf = (int *)malloc(256*1024);
	result = (int *)malloc(4096);
	memset(result, 0, 4096);
	read(0,buf, 256*1024);
	for(int i=0;i<64;i++){
		for(int j=0;j<1024;j++){
			result[j] += buf[i*1024+j];
		}
	}
	for(int i=0;i<64;i++){
		printf("%d ", result[i]);
	}
	printf("\n");
}
