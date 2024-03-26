#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <fcntl.h> 
#include <linux/fb.h> 
#include <sys/mman.h> 
#include <sys/ioctl.h> 
 
#define PAGE_SIZE 	4096
#define BUF_SIZE  	PAGE_SIZE
#define OFFSET_0	0
#define OFFSET_1	PAGE_SIZE

int main(int argc , char *argv[]) 
{ 
	int fd; 
	int i; 
	unsigned char *p_map_0; 
	unsigned char *p_map_1; 
 
	fd = open("/dev/mymap",O_RDWR); 
	if(fd < 0) 
	{ 
		printf("open fail\n"); 
		exit(1); 
	} 
 
	p_map_0 = (unsigned char *)mmap(0, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd, OFFSET_0); 
	if(p_map_0 == MAP_FAILED) 
	{ 
		printf("mmap 0 fail\n"); 
		munmap(p_map_0, PAGE_SIZE); 
		return -1;
	} 
 	
	p_map_1 = (unsigned char *)mmap(0, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,fd, OFFSET_1); 
	if(p_map_1 == MAP_FAILED) 
	{ 
		printf("mmap 1 fail\n"); 
		munmap(p_map_1, PAGE_SIZE); 
		return -1;
	} 
 

	for(i=0;i<10;i++)
		printf("%d %d\n",p_map_0[i],p_map_1[i]); 
	
	return 0; 
} 
