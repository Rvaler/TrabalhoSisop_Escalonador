
/*
 *	Programa de exemplo de uso da biblioteca sthread
 *
 *	Versão 1.0 - 25/03/2015
 *
 *	Sistemas Operacionais I - www.inf.ufrgs.br
 *
 */

#include "../include/mthread.h"

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

int main(int argc, char *argv[]) {

    int	id0, id1;
	int i;

    id0 = mcreate(0, func0, (void *)&i);
    id1 = mcreate(1, func1, (void *)&i);

    printf("Eu sou a main após a criação de ID0 e ID1\n");

    mwait(id0);
    mwait(id1);

    printf("Eu sou a main voltando para terminar o programa\n");
}
