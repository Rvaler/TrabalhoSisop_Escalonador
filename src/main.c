#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"


int global = 0;

mmutex_t mutex;

void func4()
{
	printf("IMMA STEAL THAT MUTEX\n");
	mlock(&mutex);
	printf("GOT IT\n");
	munlock(&mutex);
	printf("LERIGOOOOO.\n");

}

void func3()
{
    printf("Lock3...\n");
    mcreate(0, func4, NULL);
    mlock(&mutex);
    printf("Yield3...\n");
    myield();
    printf("Unlock3...\n");
    munlock(&mutex);
}

void func1()
{
	printf("Lock1...\n");
	mlock(&mutex);
	printf("Yield1...\n");
	myield();
    mcreate(0, func3, NULL);
    myield();
	printf("Unlock1...\n");
    munlock(&mutex);
}

void func2()
{
	printf("Lock2...\n");
	mlock(&mutex);
    printf("Mutex flag: %d\n", mutex.flag);
	printf("Yield2...\n");
	myield();
	printf("Unlock2...\n");
    munlock(&mutex);
    printf("Mutex flag after unlock: %d\n", mutex.flag);
}

int main()
{
    int tid1, tid2;
    tid1 = mcreate(0, func1, NULL);
	tid2 = mcreate(1, func2, NULL);
	//mmutex_init(&mutex);
	//mlock(&mutex);
    myield();
    //FuncaoParaTeste();

    mwait(tid1);
    mwait(2);

	//swait(tid2);
    return 0;
}
