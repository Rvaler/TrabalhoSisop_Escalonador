#include "../include/mthread.h"
#include "../include/mdata.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h> //usada para malloc

int tid = 1;                 //global var, tid of the thread
int n_threads = 0;           //number of threads

ucontext_t exit_context; //all threads, when finished, are redirected to this context

TCB_t *mainThread = NULL;

//initialize three priority queues:
TCB_t* tcbQueueLow = NULL;
TCB_t* tcbQueueMedium = NULL;
TCB_t* tcbQueueHigh = NULL;

//current thread running in the core
TCB_t* runningThread = NULL;
//the last thread removed from the running state
//TCB_t *removedFromRunning = NULL;
/*
typedef struct node {
    int val;
    struct node * next;
} node_t;

typedef struct TCB {
	int  tid;		// identificador da thread
	int  state;	// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	int  prio;		// prioridade da thread (0:alta; 1: média, 2:baixa)
	ucontext_t*   context;	// contexto de execução da thread (SP, PC, GPRs e recursos)
	struct TCB   *prev;		// ponteiro para o TCB anterior da lista
	struct TCB   *next;		// ponteiro para o próximo TCB da lista
} TCB_t;
*/




void FuncaoParaTeste(){

    printf("\nLISTA  alta priori --------");
    printQueue(tcbQueueHigh);

    printf("\nLISTA  media priori --------");
    printQueue(tcbQueueMedium);

    printf("\nLISTA  baixa priori --------");
    printQueue(tcbQueueLow);

    printf("\nLISTA  media priori --------");
    printQueue(tcbQueueHigh);
    printf("\nrunning thread: %i", runningThread->tid);
    tcbQueueHigh = dequeue(tcbQueueHigh, &runningThread);
    printf("\nrunning thread: %i", runningThread->tid);
    printf("\nLISTA  media priori --------");
    printQueue(tcbQueueHigh);

    return;
/* TESTE YIELD
    TCB_t *teste = (TCB_t*) malloc(sizeof(TCB_t));
    teste->prio = 0;
    teste->tid = 69;
    runningThread = teste;

    myield();
    printQueue(tcbQueueHigh);
    return;
*/

}

void scheduler(){

    TCB_t *wasRunning = NULL;
    TCB_t *choosenThread = NULL;
    //removedFromRunning = runningThread;


    ///verify each queue for an thread in the READY_STATE, if find, assign it to "choosenThread"
    //if (tcbQueueHigh != NULL){
    if(0){
        tcbQueueHigh = dequeue(tcbQueueHigh, &choosenThread);
        printf("\nentrando na high");
    }else if(tcbQueueMedium != NULL){
        tcbQueueMedium = dequeue(tcbQueueMedium, &choosenThread);
        printf("\nentrando na medium");
    }else if(tcbQueueLow != NULL){
        tcbQueueLow = dequeue(tcbQueueLow, &choosenThread);
        printf("\nentrando na low");
    }

    printf("\nchoosen: %i", choosenThread->tid);
    printf("\nrunning: %i", runningThread->tid);
    wasRunning = runningThread;
    runningThread = choosenThread;
    printf("\nchoosen: %i", choosenThread->tid);
    printf("\nrunning: %i", runningThread->tid);

    if (wasRunning != NULL){
        swapcontext(wasRunning->context, choosenThread->context);
        return;
    }else{
        setcontext(wasRunning->context);
        printf("\nchoosen: %i", choosenThread->tid);
    printf("\nrunning: %i", runningThread->tid);
        return;
    }

}

int createMainThread()
{
    mainThread = (TCB_t*) malloc(sizeof(TCB_t));
    if (mainThread == NULL){
        return -1;
    }

    mainThread->state = RUNNING_STATE;
    mainThread->prio = 0;
    mainThread->tid = 0;
    mainThread->next = NULL;
    mainThread->prev = NULL;

    if ((mainThread->context = (ucontext_t*) malloc(sizeof(ucontext_t))) == NULL){
        return -1;
    }

    runningThread = mainThread;
    getcontext(mainThread->context);
    return 0;
}

/*
    Creation of a new Thread and put it on ready state
*/
int mcreate(int prio, void (*start)(void*), void * arg)
{
    TCB_t *newThread = (TCB_t*) malloc(sizeof(TCB_t));
    if (newThread == NULL){
        return -1;
    }

    newThread->tid = tid++;
    newThread->state = READY_STATE;
    newThread->context = (ucontext_t*) malloc(sizeof(ucontext_t));
    if(newThread->context == NULL){
        return -1;
    }

    getcontext(newThread->context);

    newThread->context->uc_link = &exit_context;
    newThread->context->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
    if(newThread->context->uc_stack.ss_sp == NULL){
        return -1;
    }
    newThread->context->uc_stack.ss_size = SIGSTKSZ;

    makecontext(newThread->context, (void(*)(void))start, 1, arg);

    newThread->prio = prio;
    newThread->next = NULL;
    newThread->prev = NULL;


    n_threads++;


    ///precisa colocar na fila de aptos aqui
    switch(prio){
        case 0: //high
            tcbQueueHigh = enqueue(tcbQueueHigh, newThread);
            break;
        case 1:
            tcbQueueMedium = enqueue(tcbQueueMedium, newThread);
            break;
        case 2:
            tcbQueueLow = enqueue(tcbQueueLow, newThread);
            break;
    }

    /// SÓ PRA TESTAR A TROCA DA THREAD QUE ESTA EXECUTANDO /// DEPOIS TIRAR
    //runningThread = newThread;



    return newThread->tid; //identificador da thread criada
}



///removes the running thread and put it on its current queue
int myield()
{

    //if there's no thread executing, error!
    if (runningThread ==  NULL)
    {
        return -1;
    }
    //printf("\nYELD");
    //printf("\nquem esta executando: %i", runningThread->tid);

    TCB_t *removedFromRunning ;
    removedFromRunning = runningThread;
    removedFromRunning->state = READY_STATE;
    printQueue(tcbQueueHigh);
    //printf("\nquem esta executando: %i", removedFromRunning->tid);
    switch(removedFromRunning->prio)
    {
        case 0: //high
            tcbQueueHigh = enqueue(tcbQueueHigh, removedFromRunning);
            break;
        case 1:
            tcbQueueMedium = enqueue(tcbQueueMedium, removedFromRunning);
            break;
        case 2:
            tcbQueueLow = enqueue(tcbQueueLow, removedFromRunning);
            break;
    }
    printQueue(tcbQueueHigh);
    /// HERE WE HAVE TO CALL THE SCHEDULER!!!!!
    scheduler();
     //printf("\nquem esta executando: %i", runningThread->tid);
    //printQueueReverse(tcbQueueMedium);
    return 0;
}


void killthread()
{
    printf("terminou thread");
}





