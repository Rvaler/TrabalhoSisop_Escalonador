#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"
/*
mmutex_t mutex;


void func1(void *arg) {

        printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));

	return;
}

void func2(void *arg) {

        printf("Eu sou a thread ID2 imprimindo %d\n", *((int *)arg));
	return;
}

void func3(void *arg) {

        printf("Eu sou a thread ID3 imprimindo %d\n", *((int *)arg));
	return;
}

void func4(void *arg) {

        printf("Eu sou a thread ID4 imprimindo %d\n", *((int *)arg));
	return;
}

void func5(void *arg) {

        printf("Eu sou a thread ID5 imprimindo %d\n", *((int *)arg));
	return;
}



int main(int argc, char *argv[])
{
    int id1, id2, id3, id4, id5;
    void* i = NULL;

    id1 = mcreate(MEDIUM_PRIORITY, func1, (void *)&i);
    //mlock(&mutex);
    id2 = mcreate(LOW_PRIORITY, func2, (void *)&i);
    mlock(&mutex);
    mlock(&mutex);
    id3 = mcreate(MEDIUM_PRIORITY, func3, (void *)&i);
    myield();
    id4 = mcreate(MEDIUM_PRIORITY, func4, (void *)&i);
    id5 = mcreate(MEDIUM_PRIORITY, func5, (void *)&i);

    myield();
    //int teste = mcreate(2, func0, (void *)&i);
    printf("\nid0 = %i", id1);
    printf("\nid1 = %i", id2);
    printf("\nid1 = %i", id3);
    return 0;
}
*/

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
