#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"

void func0(void *arg) {

        printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));

	return;
}

void func1(){
    printf("lala\n");
    return;
}

int main(int argc, char *argv[])
{
    int id0, id1, id2, id3, id4;
    void* i = NULL;
    createMainThread();
    id0 = mcreate(1, func0, (void *)&i);
    id1 = mcreate(2, func1, (void *)&i);
    id2 = mcreate(1, func1, (void *)&i);

    id3 = mcreate(1, func1, (void *)&i);
    id4 = mcreate(1, func1, (void *)&i);

    myield();
    //int teste = mcreate(2, func0, (void *)&i);
    printf("\nid0 = %i", id0);
    printf("\nid1 = %i", id1);
    printf("\nid1 = %i", id2);
    return 0;
}
