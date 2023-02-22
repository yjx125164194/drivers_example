#include <stdio.h>

void helloworld(void)
{
	printf("helloworld\n");
}

void helloworld2(void)
{
	printf("helloworld2\n");
}

int main()
{
	void (*func)(void) = NULL;
	void (**func2)(void) = NULL;
	
	func = helloworld;
	printf("func2 %p,func2 point %p,func %p,func point %p\n",&func2,func2,&func,func);
	func2 = &func;
	func = helloworld2;
	printf("func2 %p,func2 point %p,func %p,func point %p\n",&func2,func2,&func,func);
	
	(*func2)();
}


