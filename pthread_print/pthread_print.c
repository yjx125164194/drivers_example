#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

int g_num = 1;
pthread_mutex_t mutex;
pthread_cond_t cond1,cond2;

void* thread1(void* arg)
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
        	printf("Thread1: %d \n",g_num);
        	g_num++;
        	pthread_cond_signal(&cond2);
        	pthread_cond_wait(&cond1,&mutex);		
		pthread_mutex_unlock(&mutex);
		usleep(100000);
	}
	return NULL;
}

void* thread2(void* arg)
{       
	//这个sleep(1)加在前面是因为开启线程时有可能是线程2先打印，
        //导致变成thread2输出奇数，threa1输出偶数。为了避免这种情况，可以在延迟下线程2的打印。
	sleep(1);

	while(1)
	{
		pthread_mutex_lock(&mutex);
        	printf("Thread2: %d \n",g_num);
		g_num++;
        	pthread_cond_signal(&cond1);
        	pthread_cond_wait(&cond2,&mutex);
		pthread_mutex_unlock(&mutex);
		usleep(100000);
	}
	return NULL;
}
int main()
{
	pthread_t p1,p2;
	
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond1,NULL);
	pthread_cond_init(&cond2,NULL);	
	
	pthread_create(&p1,NULL,thread1,NULL);
	pthread_create(&p2,NULL,thread2,NULL);
    	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond1);
	pthread_cond_destroy(&cond2);

	return 0;
}
