#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"

mmutex_t mutex;


void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	mlock(&mutex);
	printf("Eu sou a thread ID0 terminando %d\n");
	return;
}

void* func1(void *arg) {
    myield();
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
	munlock(&mutex);
	printf("Eu sou a thread ID1 terminando %d\n");

}

int main(int argc, char *argv[]) {

    int	id0, id1;
	int i;

	mmutex_init(&mutex);
	mlock(&mutex);

    id0 = mcreate(0, func0, (void *)&i);
    id1 = mcreate(0, func1, (void *)&i);

    printf("Eu sou a main após a criação de ID0 e ID1\n");

    myield();
    mwait(id0); //wait por thread que foi bloqueada por mutex
    //mwait(id1);

    printf("Eu sou a main voltando para terminar o programa\n");
}
d
