#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
	int fd;
	char buf[5000];
	int cnt;
	int ret = 0;
	fd = open("./read.txt", O_RDONLY);
	for(int i=0;i<8;i++){
		cnt = 0;
		read(fd, buf, 4096);
		for(int j=0;j<4096;j++){
			if (buf[j] != ' '){
				cnt++;
			}
		}
		printf("%d\n",cnt);
		ret += cnt;
	}
	printf("result: %d\n", ret);
	return 0;
}
