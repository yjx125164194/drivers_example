#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define POW(c) (1 << (c))
#define MASK(c) (((unsigned long) - 1) / (POW(POW(c)) + 1))
#define ROUND(n,c) (((n) & MASK(c)) + ((n) >> POW(c) & MASK(c)))

void print_binary(unsigned int n)
{
	if(n >> 1)
		print_binary(n >> 1);

	printf("%d",(n & 1)?1:0);
}

void print_all(unsigned int n,unsigned char *s)
{
	printf("%s\n",s);
	printf("0x%x\n",n);
	printf("0D%d\n",n);
	printf("0b");
	print_binary(n);
	printf("\n");
}

int main(int argc, char *argv[])
{
	int opt;
	unsigned int n;
	while ((opt = getopt(argc, argv, "s:")) != -1) {
		switch (opt) {
		case 's':
			n = atoi(optarg);
			break;
		default:
			printf("usage: %s [-s]\n", argv[0]);
			exit(1);
		}
	}

	print_all(n,"input data");
#if 0
	print_all(MASK(0),"MASK0");
	print_all(MASK(1),"MASK1");
	print_all(MASK(2),"MASK2");
	print_all(MASK(3),"MASK3");
	print_all(MASK(4),"MASK4");
#endif 
	n = ROUND(n,0);
	print_all(n,"ROUND 0");
	n = ROUND(n,1);
	print_all(n,"ROUND 1");
	n = ROUND(n,2);
	print_all(n,"ROUND 2");
	n = ROUND(n,3);
	print_all(n,"ROUND 3");
	n = ROUND(n,4);
	print_all(n,"ROUND 4");
	
	return 0;
}
