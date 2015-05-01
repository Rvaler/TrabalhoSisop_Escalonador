#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"

/*
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
*/
int	id0, id1, id2, id3, id4;

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));

	return;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
	mwait(id3);
}

void* func2(void *arg) {
	printf("lalalalalalalallalaallalalLALALLAALL %d\n", *((int *)arg));
	mwait(id3);
}
void* func3(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}
void* func4(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
	mwait(id3);
}

int main(int argc, char *argv[]) {


	int i;

    id0 = mcreate(0, func0, (void *)&i);
    id1 = mcreate(1, func1, (void *)&i);
    id2 = mcreate(1, func2, (void *)&i);
    id3 = mcreate(2, func3, (void *)&i);
    id4 = mcreate(1, func4, (void *)&i);


    printf("Eu sou a main após a criação de ID0 e ID1\n");

    mwait(id0);
    mwait(id2);

    //mwait(id1);

    printf("Eu sou a main voltando para terminar o programa\n");
    return 0;
}
