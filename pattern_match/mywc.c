#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define target_len 10

int main(){
	int fd;
	char buf[40000];
	char target_data[target_len] = "Lorem";
	int cnt = 0;
	fd = open("./data.txt", O_RDONLY);
	read(fd, buf, 32768);
	for(int i=0;i<32768-target_len;i++){
		if(buf[i] == target_data[0]){
			if(strncmp(buf+i, target_data, 5) == 0){
				cnt++;
			}
		}
	}
	printf("%s: %d\n",target_data, cnt);
	return 0;
}
