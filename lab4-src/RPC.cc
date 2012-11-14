//
// CS354: Implementaiton of the interprocess RPC
//

#include "RPC.h"
#include <sys/mman.h>
#include <pthread.h>
#include <thread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h> // for later checking safety memory management

RPCServer::RPCServer( char * sharedFileName, int maxAvailableThreads )
{
	// 1. Create a file sharedFileName for RPCShared of the size we need for maxAvailableThreads
	int file = open(sharedFileName, O_RDWR | O_CREAT | O_TRUNC, 0660);
	lseek(file,  (sizeof(RPCShared) + (sizeof(RPCThread) * (maxAvailableThreads-1))), SEEK_SET);
	write(file, "0", 1); // put delimiter to end of file size
	
	// 2. mmap this file using the MAP_SHARED flag so changes in memory will be done in the file.
	// See mmap man pages
	_shared = (RPCShared*)mmap(NULL, (sizeof(RPCShared) + (sizeof(RPCThread) * (maxAvailableThreads-1))), PROT_READ|PROT_WRITE, MAP_SHARED, file, 0);
	if(_shared == MAP_FAILED){
	  perror("mmapFailure");
	  exit(2);
	}
	_shared->_maxAvailableThreads = maxAvailableThreads;
	_shared->_totalCalls = 0;
	_shared->_maxSimultaneousCalls = 0;
	_shared->_currentSimultaneousCalls = 0;
	
	// 3. Initialze semaphores and mutex. Use the USYNC_PROCESS flags since semaphores and mutexs
	//    will be used to synchroinize threads across processes.
	mutex_init(&_shared->_mutex, USYNC_PROCESS, NULL);
// 	sema_init(&_shared->_sem_available_threads, maxAvailableThreads, USYNC_PROCESS, NULL);
	cond_init(&_shared->_cond_available_threads, USYNC_PROCESS, NULL); 
	
	// 4. For each entry in the array rpcThread also initialize the semaphores and entries.
	int i;
	for(i = 0 ; i <= maxAvailableThreads-1 ; i++){
	  _shared->_rpcThread[i]._in_use = false;
	  _shared->_rpcThread[i]._incoming_request = 0;
	  _shared->_rpcThread[i]._result_ready = 0;
// 	  sema_init(&_shared->_rpcThread[i]._sem_request, 0, USYNC_PROCESS, NULL);
// 	  sema_init(&_shared->_rpcThread[i]._sem_result, 0, USYNC_PROCESS, NULL);
	  cond_init(&_shared->_rpcThread[i]._cond_request, USYNC_PROCESS, NULL);
	  cond_init(&_shared->_rpcThread[i]._cond_result, USYNC_PROCESS, NULL);
	  mutex_init(&_shared->_rpcThread[i]._mutexReqRes, USYNC_PROCESS, NULL);
	}
	
	close(file);
}

void
RPCServer::registerRPC( char * name, RPCProcedure rpcProcedure, void * cookie )
{
	// Add an entry to the _rpcTable with name, rpcProcedure and cookie.
	RPCTableEntry table;
	strcpy(table._rpcName, name);
	table._cookie = cookie;
	table._rpcProcedure = rpcProcedure;
	_rpcTable[name] = table;
}

void
RPCServer::runServer()
{
	//1. For each maxAvailableThreads-1 call pthread_create to
	//start a server thread calling threadServer  with the corresponding index 
	int i;
	pthread_attr_t attr;
	for(i = 0 ; i < _shared->_maxAvailableThreads - 1 ; i++){
	  ThreadServerArgs *temp = new ThreadServerArgs();
	  pthread_attr_init(&attr);
	  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	  temp->s = this;
          temp->threadIndex = i;
	  pthread_create( 0, &attr, (void * (*)(void *)) threadServer, (void*)temp ); 
	}
	
	
	//see RPC.h in RPCShared for getting the structure of _shared 
	//2. then call threadServer
	ThreadServerArgs *toBeReturned = new ThreadServerArgs();
	toBeReturned->s = this;
	toBeReturned->threadIndex = i;
	threadServer(toBeReturned);
}


void
RPCServer::threadServer( ThreadServerArgs * args )
{
	while (1) {
		RPCThread *t1 = &args->s->_shared->_rpcThread[args->threadIndex];
		// 1. wait for an incoming request for this thread
		mutex_lock(&t1->_mutexReqRes);
		
// 		sema_wait(&t1->_sem_request);
		while(t1->_incoming_request == 0){
		  cond_wait(&t1->_cond_request, &t1->_mutexReqRes);
		}
		// 2. lookup the procedure in the rpctable
		t1->_incoming_request = 0;
		mutex_unlock(&t1->_mutexReqRes);
		
		RPCTableEntry tableEntry = args->s->_rpcTable[t1->_rpcName];
		RPCProcedure *proc = &tableEntry._rpcProcedure;
		// 3. Invoke procedure through a pointer
		(*proc)(tableEntry._cookie, t1->_argument, t1->_result);
		// 4. sema_post that the call is complete.
		mutex_lock(&t1->_mutexReqRes);
// 		sema_post(&t1->_sem_result);
		cond_signal(&t1->_cond_result);
		t1->_result_ready = 1;
		mutex_unlock(&t1->_mutexReqRes);
	}
}

RPCClient::RPCClient( char * sharedFileName )
{
	// 1. Memory map the existing file
	int file = open(sharedFileName, O_RDWR, 0660);
	int size = lseek(file, 0, SEEK_END);
	_shared = (RPCShared*) mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, file, 0);
	close(file);
}

int
RPCClient::call( char * rpcName, RPCArgument argument, RPCResult result )
{
	// 1. Wait if there are no threads available
	mutex_lock(&_shared->_mutex);
// 	sema_wait(&_shared->_sem_available_threads);
	while(_shared->_currentSimultaneousCalls >= _shared->_maxAvailableThreads) {
	  cond_wait(&_shared->_cond_available_threads, &_shared->_mutex);
	}
 	// 2. Get mutex
	// 3. Find the index of an available thread
	int i =0;
	for(i = 0 ; i < _shared->_maxAvailableThreads ; i++){
	  if(_shared->_rpcThread[i]._in_use == false){
	    _shared->_rpcThread[i]._in_use = true;
	    _shared->_currentSimultaneousCalls++;
	    break;
	  }
	}
	
	// 4. release mutex
	mutex_unlock(&_shared->_mutex);
	
	// 4. Copy argument into the RPCThread record for that index
	memcpy(_shared->_rpcThread[i]._argument, argument, sizeof(RPCArgument));
	strcpy(_shared->_rpcThread[i]._rpcName, rpcName);
	
	mutex_lock(&_shared->_rpcThread[i]._mutexReqRes);
	// 5. Wake up that server thread
// 	sema_post(&_shared->_rpcThread[i]._sem_request);
	cond_signal(&_shared->_rpcThread[i]._cond_request);
	_shared->_rpcThread[i]._incoming_request = 1;
	
	mutex_unlock(&_shared->_rpcThread[i]._mutexReqRes);
	
	
	
	// 6.. wait until results are ready.
	mutex_lock(&_shared->_rpcThread[i]._mutexReqRes);
// 	sema_wait(&_shared->_rpcThread[i]._sem_result);
	while(_shared->_rpcThread[i]._result_ready == 0){
	  cond_wait(&_shared->_rpcThread[i]._cond_result, &_shared->_rpcThread[i]._mutexReqRes);
	}
	_shared->_rpcThread[i]._result_ready = 0;
	mutex_unlock(&_shared->_rpcThread[i]._mutexReqRes);
	
	memcpy(result, _shared->_rpcThread[i]._result, sizeof(RPCResult));
	
	// 7. return
	mutex_lock(&_shared->_mutex);
	if(_shared->_maxSimultaneousCalls < _shared->_currentSimultaneousCalls)
	  _shared->_maxSimultaneousCalls = _shared->_currentSimultaneousCalls;
	
	_shared->_currentSimultaneousCalls--;
	_shared->_totalCalls++;
	_shared->_rpcThread[i]._in_use = false;
	
// 	sema_post(&_shared->_sem_available_threads);
	cond_signal(&_shared->_cond_available_threads);
	
	mutex_unlock(&_shared->_mutex);
	
	
  return 0;
}

int 
RPCClient::maxSimultaneousCalls()
{
  return _shared->_maxSimultaneousCalls;
}

int
RPCClient::totalCalls()
{
  return _shared->_totalCalls;
}