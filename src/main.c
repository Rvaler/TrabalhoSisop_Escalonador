#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"

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
