#include "../include/mthread.h"
#include "../include/mdata.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h> //usada para malloc

int tid = 1;                 //global var, tid of the thread
int n_threads = 0;           //number of threads
int createdMain = 0;
ucontext_t* exit_context = NULL; //all threads, when finished, are redirected to this context

TCB_t *mainThread = NULL;

//initialize three priority queues:
TCB_t* tcbQueueLow = NULL;
TCB_t* tcbQueueMedium = NULL;
TCB_t* tcbQueueHigh = NULL;

TCB_t* blockedQueue = NULL;

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

void killthread()
{
    printf("\nterminou thread com tid: ");
    printf("%i", runningThread->tid);

    free(runningThread->context->uc_stack.ss_sp);
    free(runningThread->context);
    free(runningThread);
    runningThread = NULL;
    scheduler();
    ///aqui mata a thread e chama o escalonador
}

void FuncaoParaTeste(){

    printf("\nLISTA  alta priori --------\n");
    printQueue(tcbQueueHigh);

    printf("\nLISTA  media priori --------\n");
    printQueue(tcbQueueMedium);

    printf("\nLISTA  baixa priori --------\n");
    printQueue(tcbQueueLow);

    /*printf("\nLISTA  media priori --------");
    printQueue(tcbQueueHigh);
    printf("\nrunning thread: %i", runningThread->tid);
    tcbQueueHigh = dequeue(tcbQueueHigh, &runningThread);
    printf("\nrunning thread: %i", runningThread->tid);
    printf("\nLISTA  media priori --------");
    printQueue(tcbQueueHigh);*/

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
    printf("\n ----- CHAMADO ESCALONADOR ------------");
    //set the context so that when a thread ends, it returns to here
    ///getcontext(&exit_context);



    TCB_t *wasRunning = NULL;
    TCB_t *choosenThread = NULL;
    //removedFromRunning = runningThread;


    ///verify each queue for an thread in the READY_STATE, if find, assign it to "choosenThread"


    FuncaoParaTeste();
    ///esse if tem que ser retirado na entrega
    if (tcbQueueHigh != NULL){
    //if(0){
        tcbQueueHigh = dequeue(tcbQueueHigh, &choosenThread);
        printf("\nentrando na lista high\n");
    }else if(tcbQueueMedium != NULL){
        tcbQueueMedium = dequeue(tcbQueueMedium, &choosenThread);
        printf("\nentrando na lista medium\n");
    }else if(tcbQueueLow != NULL){
        tcbQueueLow = dequeue(tcbQueueLow, &choosenThread);
        printf("\nentrando na lista low\n");
    }else {
        printf("\n\n####Todas filas vazias\n");
        //TODO
        //o que faz quando todas listas estão vazias?
    }
    //FuncaoParaTeste();

    //printf("\nchoosen: %i", choosenThread->tid);
    //printf("\nrunning: %i", runningThread->tid);
    wasRunning = runningThread;
    runningThread = choosenThread;
    //printf("\nchoosen: %i", choosenThread->tid);
    //printf("\nrunning: %i", runningThread->tid);

    if (wasRunning != NULL){
        printf("\n\nescaloonador vai fazer o swapcontext");
        swapcontext(wasRunning->context, choosenThread->context);

        return;
    }else{
        printf("\n\n escalonador vai setar o contexto");
        setcontext(choosenThread->context);
        //printf("\nchoosen: %i", choosenThread->tid);
    //printf("\nrunning: %i", runningThread->tid);
        return;
    }



}

//Parâmetros
//  tid: identificador da thread cujo término está sendo aguardado.
//Retorno:
//  Retorna o valor 0 (zero, se a função foi realizada com sucesso; caso contrário,  retorna -1.

int mwait(int tid){


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
    createdMain = 1;
    printf("\n---CRIADA MAIN THREAD----\n");
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

    //creates the exit_Context
    if (exit_context == NULL)
    {
        printf("criou exitcontext");
        exit_context = (ucontext_t*) malloc(sizeof(ucontext_t));
        if (exit_context== NULL)
            return -1;
        exit_context->uc_link = NULL;
        exit_context->uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
        exit_context->uc_stack.ss_size = SIGSTKSZ;

        getcontext(exit_context);
        makecontext(exit_context, (void (*)(void)) killthread, 0, NULL);
    }




    newThread->tid = tid++;
    newThread->state = READY_STATE;
    newThread->context = (ucontext_t*) malloc(sizeof(ucontext_t));
    if(newThread->context == NULL){
        return -1;
    }

    getcontext(newThread->context);

    newThread->context->uc_link = exit_context;
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

    ///creation of Main in the first time of execution
    if(createdMain == 0){
        //if main thread created successful
        if(createMainThread() == 0){
            return newThread->tid;
        }else{
            return -1;
        }
    }


    return newThread->tid; //identificador da thread criada
}



///removes the running thread and put it on its current queue
int myield()
{
    printf("\n ----- YIELD ------------");
    //if there's no thread executing, error!
    if (runningThread ==  NULL)
    {
        return -1;
    }
    printf("\n quem chamou YIELD: %i\n", runningThread->tid);
    //printf("\nYELD");
    //printf("\nquem esta executando: %i", runningThread->tid);

    TCB_t *removedFromRunning ;
    removedFromRunning = runningThread;
    removedFromRunning->state = READY_STATE;
    //printQueue(tcbQueueHigh);
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
    //printQueue(tcbQueueHigh);
    /// HERE WE HAVE TO CALL THE SCHEDULER!!!!!
    scheduler();
     //printf("\nquem esta executando: %i", runningThread->tid);
    //printQueueReverse(tcbQueueMedium);
    return 0;
}


int mmutex_init(mmutex_t *mtx){

    if(mtx){
            printf("criou mutex");
        mtx->flag = FREE_MUTEX;
        mtx->first = NULL;
        mtx->last = NULL;
        return 0;
    }

    printf("nao criou mutex");
    return -1;
}


int mlock (mmutex_t *mtx){

    printf("\n\n-----CHAMADO O MLOCK------\n\n");
    FuncaoParaTeste();
    if (mtx->flag == FREE_MUTEX){
        printf("\nMUTEX TAVA LIVRE\n");
        mtx->flag = OCCUPIED_MUTEX;
        return 0;

    }else{

        do
        {
            printf("\nMUTEX TAVA OCUPADO\n");
            runningThread->state = BLOCKED_STATE;
            enqueue(runningThread, &mtx->first);
            mtx->last = runningThread;
            scheduler();
        }while(mtx->flag == OCCUPIED_MUTEX);
        mtx->flag = OCCUPIED_MUTEX;
        return 0;
    }
}







