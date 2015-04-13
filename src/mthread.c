#include "../include/mthread.h"
#include "../include/mdata.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h> //usada para malloc

int tid = 1;                 //global var, tid of the thread
int n_threads = 0;           //number of threads
ucontext_t main_context; //future main_context (dont know if this is necessary)
ucontext_t exit_context; //all threads, when finished, are redirected to this context


//initialize three priority queues:
TCB_t* tcbQueueLow = NULL;
TCB_t* tcbQueueMedium = NULL;
TCB_t* tcbQueueHigh = NULL;

//current thread running in the core
TCB_t* runningThread = NULL;


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

    ucontext_t context;
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

    //printQueueReverse(tcbQueueHigh);
    //printf("\ntid da thread %i", tid);
    //printf("\ntid da thread %i", newThread->tid);
    return newThread->tid; //identificador da thread criada
}

void FuncaoParaTeste(){

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

///removes the running thread and put it on its current queue
int myield()
{
    //if there's no thread executing, error!
    if (runningThread ==  NULL)
    {
        return -1;
    }
    TCB_t *removedFromRunning;
    removedFromRunning = runningThread;
    removedFromRunning->state = READY_STATE;

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

    /// HERE WE HAVE TO CALL THE SCHEDULER!!!!!

    return 0;
}


void killthread()
{
    printf("terminou thread");
}





