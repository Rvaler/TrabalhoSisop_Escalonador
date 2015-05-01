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

//current thread running in the core
TCB_t* runningThread = NULL;


waitingStruct_t* waitingList = NULL;

void pushThreadToMutex(mmutex_t *mutex, TCB_t *newWaitingTCB);


void FuncaoParaTeste(){

    printf("\nLISTA  alta priori --------\n");
    printQueue(tcbQueueHigh);

    printf("\nLISTA  media priori --------\n");
    printQueue(tcbQueueMedium);

    printf("\nLISTA  baixa priori --------\n");
    printQueue(tcbQueueLow);

    return;
}

void scheduler(){
    printf("\n ----- CHAMADO ESCALONADOR ------------\n");
    //set the context so that when a thread ends, it returns to here

    TCB_t *wasRunning = NULL;
    TCB_t *choosenThread = NULL;

    ///verify each queue for an thread in the READY_STATE, if find, assign it to "choosenThread"

    if (tcbQueueHigh != NULL){
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
        ///TODO
        ///o que faz quando todas listas estão vazias?
    }

    wasRunning = runningThread;
    runningThread = choosenThread;

    if (wasRunning != NULL){
        printf("\n\nEscaloonador vai fazer o swapcontext\n");
        printf("\nQuem estava rodando: %i \nQuem vai rodar: %i\n", wasRunning->tid, choosenThread->tid);
        swapcontext(wasRunning->context, choosenThread->context);
        return;
    }else{
        printf("\n\nEscalonador vai setar o contexto");
        printf("\nQuem vai rodar: %i\n", choosenThread->tid);
        setcontext(choosenThread->context);
        return;
    }



}


void killthread()
{
    printf("\nTerminou thread com tid: ");
    printf("%i", runningThread->tid);

    //adicioner as listas threads que liberaram com esse id
    waitingStruct_t* returnedData = NULL;
    waitingList = removeThread(waitingList, runningThread->tid, &returnedData);

    if (returnedData != NULL){
        printf("\nThread %i liberou a thread %i \n!!", runningThread->tid,returnedData->blockedThread->tid);
        printWaitingList(waitingList);
        switch(returnedData->blockedThread->prio){
        case 0: //high
            tcbQueueHigh = enqueue(tcbQueueHigh, returnedData->blockedThread);
            break;
        case 1:
            tcbQueueMedium = enqueue(tcbQueueMedium, returnedData->blockedThread);
            break;
        case 2:
            tcbQueueLow = enqueue(tcbQueueLow, returnedData->blockedThread);
            break;
        }
    }



    free(runningThread->context->uc_stack.ss_sp);
    free(runningThread->context);
    free(runningThread);
    runningThread = NULL;
    scheduler();

}

//Parâmetros
//  tid: identificador da thread cujo término está sendo aguardado.
//Retorno:
//  Retorna o valor 0 (zero, se a função foi realizada com sucesso; caso contrário,  retorna -1.

int mwait(int tid){

    printf("\n --- CHAMADO O WAIT --- \n");
    printf("\nThread que estava executando: %i", runningThread->tid);

    TCB_t* waitingThisThread = NULL;
    TCB_t* auxiliarTCB = NULL;

    if (runningThread ==  NULL)
    {
        printf("\nFalha no mwait!!");
        return -1;

    }else{

        if(tcbQueueHigh != NULL){

            auxiliarTCB = tcbQueueHigh;
            while(auxiliarTCB != NULL){

                if(auxiliarTCB->tid == tid){
                    waitingThisThread = auxiliarTCB;

                    break;
                }else{
                    auxiliarTCB = auxiliarTCB->next;
                }
            }
        }

        if(tcbQueueMedium != NULL){

            auxiliarTCB = tcbQueueMedium;
            while(auxiliarTCB != NULL){

                if(auxiliarTCB->tid == tid){
                    waitingThisThread = auxiliarTCB;
                    break;
                }else{
                    auxiliarTCB = auxiliarTCB->next;
                }
            }
        }


        if(tcbQueueLow != NULL){

            auxiliarTCB = tcbQueueLow;
            while(auxiliarTCB != NULL){

                if(auxiliarTCB->tid == tid){
                    waitingThisThread = auxiliarTCB;
                    break;
                }else{
                    auxiliarTCB = auxiliarTCB->next;
                }
            }
        }

        if(waitingThisThread == NULL){
            printf("\nFalha no MWait - nao existe thread com este tid");
            return -1;
        }

    }

    TCB_t *removedFromRunning ;
    removedFromRunning = runningThread;
    removedFromRunning->state = BLOCKED_STATE;

    waitingStruct_t *newBlockedThread = (waitingStruct_t*) malloc(sizeof(waitingStruct_t));
    if(newBlockedThread == NULL){
        printf("\nFalha no MWait - nao tem memoria disponivel\n");
        return -1;
    }

    newBlockedThread->waitedThreadTid = tid;
    newBlockedThread->blockedThread = removedFromRunning;
    printf("\nThread %i esta esperando a thread %i acabar! \n", removedFromRunning->tid, tid);
    waitingList = pushThread(waitingList, newBlockedThread);

    runningThread = NULL;
    scheduler();
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
int mcreate(int prio, void *(*start)(void*), void * arg)
{
    printf("\n ---- CHAMADO O MCREATE ----- \n");

    TCB_t *newThread = (TCB_t*) malloc(sizeof(TCB_t));
    if (newThread == NULL){
        printf("\nErro na criação da thread!! \n");
        return -1;
    }

    //creates the exit_Context
    if (exit_context == NULL)
    {
        printf("\nCriou Exit Context\n");
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

    printf("\nThread com tid %i foi criada e colocada na lista de aptos\n", newThread->tid);
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
    printf("\n ----- CHAMADO O YIELD ------------");
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
    printf("\nthread com tid %i foi colocada na lista de aptos\n");
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
    printf("\n\n-----CHAMADO O M_INIT ------\n\n");
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

    if (mtx->flag == FREE_MUTEX){
        printf("\nMUTEX TAVA LIVRE\n");
        mtx->flag = OCCUPIED_MUTEX;
        return 0;

    }else{

        do
        {
            printf("\nMUTEX TAVA OCUPADO\n");
            runningThread->state = BLOCKED_STATE;
            printf("\nthread com tid %i colocada na lista de bloqueados do mutex\n");
            //deve colocar thread atual na lista de threads ocupadas por este mutex
            pushThreadToMutex(mtx, runningThread); //essa função funcionou, e a outra não.... hehe

            //enqueue(mtx->first, runningThread);
            //mtx->last = runningThread;
            //printQueue(mtx->first);
            scheduler();
        }while(mtx->flag == OCCUPIED_MUTEX);
        mtx->flag = OCCUPIED_MUTEX;
        return 0;
    }
}

int munlock (mmutex_t *mtx){

    printf("\n\n-----CHAMADO O MUNLOCK------\n\n");
    if (mtx->first != NULL) {

        TCB_t *firstOfMutex = NULL;
        mtx->first = dequeue(mtx->first, &firstOfMutex);
        firstOfMutex->state = READY_STATE;
        printf("\nthread com tid %i colocada na lista de aptos\n", firstOfMutex->tid);
        switch(firstOfMutex->prio)
        {
            case 0: //high
                tcbQueueHigh = enqueue(tcbQueueHigh, firstOfMutex);
                break;
            case 1:
                tcbQueueMedium = enqueue(tcbQueueMedium, firstOfMutex);
                break;
            case 2:
                tcbQueueLow = enqueue(tcbQueueLow, firstOfMutex);
                break;
        }
    }

    mtx->flag = FREE_MUTEX;
    return 0;

}





void pushThreadToMutex(mmutex_t *mutex, TCB_t *newWaitingTCB){
    //TCB_t *ptAuxPrevious = NULL; //auxiliar pointer to precious
    TCB_t* ptAux = mutex->first;        //pointer to go trough queue

    if (isEmpty(ptAux)){
        mutex->first = newWaitingTCB; //set first position
        mutex->first->next = NULL;
        //return tcbQueue;
    } else {
        while(ptAux->next != NULL){ //cycle to the last element
            ptAux = ptAux->next;
        }

        //set the next of the last element (a new one)
        newWaitingTCB->next = NULL;
        ptAux->next = newWaitingTCB;
        //adjust previous of last element
        newWaitingTCB->prev = ptAux;
        mutex->last = newWaitingTCB;

    }
}







