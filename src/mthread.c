#include "../include/mthread.h"
#include "../include/mdata.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

void FuncaoParaTeste(void);
void pushThreadToMutex(mmutex_t *mutex, TCB_t *newWaitingTCB);
int createMainThread();

int tid = 1;                 //variável global que gerencia o tid das threads criadas
int createdMain = 0;        // flag que controla a criação da thread main
ucontext_t* exit_context = NULL; //all threads, when finished, are redirected to this context

TCB_t *mainThread = NULL;

//initialize three priority queues:
TCB_t* tcbQueueLow = NULL;
TCB_t* tcbQueueMedium = NULL;
TCB_t* tcbQueueHigh = NULL;

//current running thread
TCB_t* runningThread = NULL;

//structure that controls the waiting blocked threads
waitingStruct_t* waitingList = NULL;





void FuncaoParaTeste(){

    //printf("\nLISTA  alta priori --------\n");
    printQueue(tcbQueueHigh);

    //printf("\nLISTA  media priori --------\n");
    printQueue(tcbQueueMedium);

    //printf("\nLISTA  baixa priori --------\n");
    printQueue(tcbQueueLow);

    return;
}

/*
    SCHEDULER

    Função que trabalha como escalonador do sistema, escolhendo qual thread
    passará ao estado "executando" quando a CPU ficar ociosa.
*/

void scheduler(){
    //printf("\n ----- CHAMADO ESCALONADOR ------------\n");

    TCB_t *wasRunning = NULL;
    TCB_t *choosenThread = NULL;

    // verifica nas listas de prioridade por uma thread para então coloca-la para executar
    // ordem de busca nas listas (alta prioridade -> media prioridade -> baixa prioridade)

    if (tcbQueueHigh != NULL){
        tcbQueueHigh = dequeue(tcbQueueHigh, &choosenThread);
        //printf("\nEscalonador entrando na lista de alta prioridade\n");
    }else if(tcbQueueMedium != NULL){
        tcbQueueMedium = dequeue(tcbQueueMedium, &choosenThread);
        //printf("\nEscalonador entrando na lista de media prioridade\n");
    }else if(tcbQueueLow != NULL){
        tcbQueueLow = dequeue(tcbQueueLow, &choosenThread);
        //printf("\nEscalonador entrando na lista de baixa prioridade\n");
    }else {
        //printf("\n\n####Todas filas vazias\n");
        ///TODO
        ///o que faz quando todas listas estão vazias?
    }

    wasRunning = runningThread;
    runningThread = choosenThread;

    //realiza a troca de contextos entre a thread que estava executando e a nova thread que executará
    if (wasRunning != NULL){
        //printf("\nEscaloonador vai fazer o swapcontext\n");
        //printf("\nQuem estava rodando: %i \nQuem vai rodar: %i\n", wasRunning->tid, choosenThread->tid);
        swapcontext(wasRunning->context, choosenThread->context);
        return;
    }else{

    //seta o contexto, isso acontece quando não existia thread executando
        //printf("\nEscalonador vai setar o contexto");
        //printf("\nQuem vai rodar: %i\n", choosenThread->tid);
        setcontext(choosenThread->context);
        return;
    }

}

/*
    KILLTHREAD

    Chamado para desalocar da memória uma thread quando sua execução termina.
    Além disso, procura se alguma thread está bloqueada pela thread que terminou,
    caso seja verdadeiro então passa a thread que estava bloqueada para o estado apto.
*/


void killthread()
{
    //printf("\nTerminou thread com tid: ");
    //printf("%i", runningThread->tid);

    //adiciona as listas threads que liberaram com esse id
    waitingStruct_t* returnedData = NULL;
    //procura nas threads bloqueadas se existia alguma thread esperando pela thread que terminou
    waitingList = removeThread(waitingList, runningThread->tid, &returnedData);

    //caso exista alguma thread esperando pela que terminou, aquela é colocada na lista de aptos
    if (returnedData != NULL){
        //printf("\nThread %i liberou a thread %i \n!!", runningThread->tid,returnedData->blockedThread->tid);
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


    //retira de memória a thread que terminou e chama o escalonador
    free(runningThread->context->uc_stack.ss_sp);
    free(runningThread->context);
    free(runningThread);
    runningThread = NULL;
    scheduler();

}

//verifica se não existem duas threads esperando por uma mesma thread
int verifyWaitingList(waitingStruct_t *wqueue, int tid){
    waitingStruct_t* ptAux = wqueue;        //pointer to go trough queue
    for (; ptAux!=NULL; ptAux=ptAux->next){
        if(tid == ptAux->waitedThreadTid){
            //printf("\nErro no MWAIT - thread ja esta sendo esperada por outra thread\n");
            return -1;
        }
    }
    return 0;
}


/*
    MWAIT:
    Thread aguarda que outra thread termine sua execução para então poder ser executada,
    até que isso ocorra a thread que chamou mwait ficará bloqueada.
    Parâmetros:
        tid: identificador da thread cujo término está sendo aguardado.
    Retorno:
        Retorna 0 se concluída com sucesso e -1 quando concluída com erro.
*/

int mwait(int tid){

    if(createdMain == 0){
        if(createMainThread() == -1){
            return -1;
        }
    }

    //printf("\n --- CHAMADO O WAIT --- \n");

    // evita que duas threads diferentes esperem pela mesma thread
    if(verifyWaitingList(waitingList, tid) == -1){
        return -1;
    }

    //printf("\nThread que estava executando: %i", runningThread->tid);

    TCB_t* waitingThisThread = NULL;
    TCB_t* auxiliarTCB = NULL;

    if (runningThread ==  NULL)
    {
        //printf("\nFalha no mwait!!");
        return -1;

    }else{

    //procura pela thread que será esperada
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


        /// tentar procurar na lista de waiting list tambem, pq uma thread pode dar wait em uma que ta bloqueada
        /// pq deu um wait tambem
        /*
        if(waitingList != NULL){
            //printf("\nARPPPP\n");
            waitingStruct_t *auxiliarWL;
            auxiliarWL = waitingList;

            while(auxiliarWL != NULL){


                if(auxiliarWL->blockedThread->tid == tid){
                    //printf("\nentrou no if\n");
                    waitingThisThread = auxiliarWL->blockedThread;
                    break;
                }else{
                    //printf("\naqui da pau\n");
                    auxiliarWL = auxiliarWL->next;
                }
            }

        }
        */


        // thread esperada pelo mwait não foi encontrada, erro!
        if(waitingThisThread == NULL){
            //printf("\nFalha no MWait - nao existe thread com este tid");
            return -1;
        }

    }

    TCB_t *removedFromRunning ;
    removedFromRunning = runningThread;
    removedFromRunning->state = BLOCKED_STATE;

    // thread que estava executando é inserida na lista de threads bloqueadas por estarem esperando
    // o término de outra thread
    waitingStruct_t *newBlockedThread = (waitingStruct_t*) malloc(sizeof(waitingStruct_t));
    if(newBlockedThread == NULL){
        //printf("\nFalha no MWait - nao tem memoria disponivel\n");
        return -1;
    }

    newBlockedThread->waitedThreadTid = tid;
    newBlockedThread->blockedThread = removedFromRunning;
    //printf("\nThread %i esta esperando a thread %i acabar! \n", removedFromRunning->tid, tid);
    waitingList = pushThread(waitingList, newBlockedThread);

    printWaitingList(waitingList);
    runningThread = NULL;
    scheduler();
    return 0;
}

/*
    Create Main Thread

    Cria a Main Thread (principal) e inicializa a mesma, seu TID será igual a 0
    e ela sera colocada na lista de alta prioridade.
*/

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
    //printf("\nCriada a main thread com tid %i\n", mainThread->tid);
    runningThread = mainThread;
    createdMain = 1;
    getcontext(mainThread->context);


    return 0;
}

/*
    MCREATE:
    Criação da thread, aloca uma TCB e a inicializa
    Parâmetros:
        prio: prioridade da thread criada (0: alta prioridade, 1: média prioridade, 2: baixa prioridade)
        start: ponteiro para a função que a thread executará
        arg: UM parâmetro que pode ser passado para a thread na sua criação.
    Retorno:
        Retorna 0 se concluída com sucesso e -1 quando concluída com erro.
*/
int mcreate(int prio, void *(*start)(void*), void * arg)
{

    if(createdMain == 0){
        if(createMainThread() == -1){
            return -1;
        }
    }

    //printf("\n ---- CHAMADO O MCREATE ----- \n");

    TCB_t *newThread = (TCB_t*) malloc(sizeof(TCB_t));
    if (newThread == NULL){
        //printf("\nErro na criação da thread!! \n");
        return -1;
    }

    //criação do contexto de término das threads
    if (exit_context == NULL)
    {
        //printf("\nCriou Exit Context\n");
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

    //printf("\nThread com tid %i foi criada e colocada na lista de aptos\n", newThread->tid);

    //inserção da thread criada na lista de aptos
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



    return newThread->tid; //identificador da thread criada
}


/*
    MYIELD:
    Thread libera voluntariamente a CPU, a thread que estava executando vai para a lista de aptos
    da sua respectiva prioridade, e então após isso o escalonador é chamado.

    Retorno:
        Retorna 0 se concluída com sucesso e -1 quando concluída com erro.
*/

int myield()
{
    if(createdMain == 0){
        if(createMainThread() == -1){
            return -1;
        }
    }

    //printf("\n ----- CHAMADO O YIELD ------------");
    //if there's no thread executing, error!
    if (runningThread ==  NULL)
    {
        return -1;
    }

    //printf("\n quem chamou YIELD: %i\n", runningThread->tid);

    TCB_t *removedFromRunning ;
    removedFromRunning = runningThread;
    removedFromRunning->state = READY_STATE;

    //printf("\nthread com tid %i foi colocada na lista de aptos\n", removedFromRunning->tid);

    //thread que estava executando é colocada na lista de aptos
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
    scheduler();
    return 0;
}

/*
    MMUTEX_INIT:
    Inicializa uma variável do tipo mmutex_t e coloca-o em estado livre, podendo ser usado.
    Parâmetros:
        mtx: ponteiro para uma variável do tipo mmutex_t.
    Retorno:
        Retorna 0 se concluída com sucesso e -1 quando concluída com erro.
*/

int mmutex_init(mmutex_t *mtx){
    //printf("\n\n-----CHAMADO O M_INIT ------\n\n");
    if(mtx){
        //printf("criou mutex");
        mtx->flag = FREE_MUTEX;
        mtx->first = NULL;
        mtx->last = NULL;
        return 0;
    }

    //printf("nao criou mutex");
    return -1;
}


/*
    MLOCK:
    Indica a entrada de uma thread na seção crítica, bloqueando o mutex para que outras threads
    não possam entrar.
    Parâmetros:
        mtx: ponteiro para uma variável do tipo mmutex_t.
    Retorno:
        Retorna 0 se concluída com sucesso e -1 quando concluída com erro.
*/
int mlock (mmutex_t *mtx){

    if(createdMain == 0){
        if(createMainThread() == -1){
            return -1;
        }
    }

    //printf("\n\n-----CHAMADO O MLOCK------\n\n");

    //se mutex estava livre, agora se torna ocupado pela thread que chamou a função
    if (mtx->flag == FREE_MUTEX){
        //printf("\nMUTEX TAVA LIVRE\n");
        mtx->flag = OCCUPIED_MUTEX;
        return 0;

    }else{

    //mutex estava ocupado, thread fica bloqueada na lista de bloqueadas pelo mutex
        do
        {
            //printf("\nMUTEX TAVA OCUPADO\n");
            runningThread->state = BLOCKED_STATE;
            //printf("\nthread com tid %i colocada na lista de bloqueados do mutex\n", runningThread->tid);
            //deve colocar thread atual na lista de threads ocupadas por este mutex
            pushThreadToMutex(mtx, runningThread);
            //enqueue(mtx->first, runningThread);
            //mtx->last = runningThread;

            scheduler();
        }while(mtx->flag == OCCUPIED_MUTEX);
        mtx->flag = OCCUPIED_MUTEX;
        return 0;
    }
}


/*
    MUNLOCK:
    Indica a saída de uma thread da seção crítica, liberando o mutex para que outras threads
    não possam entrar.
    Parâmetros:
        mtx: ponteiro para uma variável do tipo mmutex_t.
    Retorno:
        Retorna 0 se concluída com sucesso e -1 quando concluída com erro.
*/
int munlock (mmutex_t *mtx){

    //printf("\n\n-----CHAMADO O MUNLOCK------\n\n");

    // coloca a primeira thread da lista de bloqueadas pelo mutex na lista de aptos
    if (mtx->first != NULL) {

        TCB_t *firstOfMutex = NULL;
        mtx->first = dequeue(mtx->first, &firstOfMutex);
        firstOfMutex->state = READY_STATE;
        //printf("\nthread com tid %i colocada na lista de aptos\n", firstOfMutex->tid);
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







