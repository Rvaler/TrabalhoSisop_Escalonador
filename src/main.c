#include <stdio.h>
#include <stdlib.h>
#include "../include/mdata.h"
#include "../include/mthread.h"

void func0(void *arg) {

        printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return;
}

void func1(){
    printf("lala");
    return;
}

int main(int argc, char *argv[])
{
    int id0, id1;
    void* i = NULL;
    id0 = mcreate(3, func0, (void *)&i);
    id1 = mcreate(1, func1, (void *)&i);
    //int teste = mcreate(2, func0, (void *)&i);
    printf("\nid0 = %i", id0);
    printf("\nid1 = %i", id1);
    return 0;
}
