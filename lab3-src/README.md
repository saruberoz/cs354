CS 354 Lab 3

Interprocess Remote Procedure Calls

Normally a program calls procedures that are defined in the program itself. In this project you will implement a mechanism to call procedures that  are implemented in other processes. A typicial use for this mechanism is a database that runs in a separate DB server and that communicates with other DB clients. The processes will use shared memory to communicate as well as shared semaphores and mutexes for synchronization.
Preparation

See the class slides related to the RPC implementation using semaphores.
How clients and servers use the RPC mechanism

First download, the file  lab3-src.tar.Z. Uncompress this file and untar it by typing:
uncompress lab3-src.tar.Z 
tar -xvf lab3-src.tar
The files client1.cc and server1.cc show how a client process calls a remote procedure that is implemented in a server process. In this case the server implements an "add"  procedure that takes as argument two integer values and gives as a result one integer.
Program  client1.cc shows how the client is implemented. It first creates a new RPCClient object called client, and passes as a parameter the name of the file "server1.rpc" that will be used to communicate between the client and the server. The arguments and the result of the RPC are stored in two buffers argBuffer, and resultBuffer, that are arrays of type character. To store the two integer arguments a and b in argBuffer, a variable arg is defined as a pointer to a structure that points  to argBuffer. The values of a and b are stored in arg->a, and arg->b. A similar technique is used to = get the result from resultBuffer. Once the arguments are in argBuffer, the function client->call( "add"...) calls the procedure  "add" remotely. After the remote procedure call returns the results are stored in resultBuffer, and can be obtained from res->a.

Program server1.cc shows how the server is implemented. It first creates a new RPCServer object called server, and passes as a parameter the name of the file "server1.rpc" that will be used to communicate between the client and the server. Then in the call server->register( "add", add, 0 ), it registers the "add" procedure add, defined previously in the program, so it can be used by the clients. Finally it calls server->runServer(), that goes into an infinite loop waiting  for calls from the clients. The procedures that the server registers need to have the same type of the add procedure. There is a buffer argument where the arguments are passed, and there is a buffer result where the add procedure stores the results after it is done. The variables arg and res are pointers that help reading or storing the values of a, b, and c in the buffers.

How client and server communicate

The clients and servers communicate through a file that is memory mapped in memory (Read the man pages for mmap). This file stores an object of type RPCShared, that has an array of _rpcThread objects of type RPCThread. A single RPCThread is used for a remote procedure call.  The number of RPCThread objects is given by _maxAvailableThreads and it gives the maximum number of simultaneous remote procedure calls. The definitions of these objects are stored in  RPC.h.
How the RPCServer object is implemented

The RPCServer object implements the following interface stored in RPC.h:
class RPCServer { 
public: 
     RPCServer( char * sharedFileName, int maxAvailableThreads ); 
     void registerRPC( char * name, RPCProcedure rpcProcedure, void * cookie ); 
     void = runServer(); 
};
RPCServer() in RPC.cc is the constructor of this object. It creates and initializes the file that will use to communicate with the clients, and maps it into memory. It will initialize also a table of the registered RPC's.
registerRPC() registers an RPC that can be called by the clients.name is the name of the procedure. rpcProcedure is the address of the procedure that implements this rpc. And cookie, is a pointer to some private data that will be passed to rpcProcedure when it is called (in client1.cc, cookie is not used and it is 0).

runsServer() implements an infinite loop that waits until a request arrives in the RPCShared. Then when it arrives it gets the arguments and executes the procedure that is being requested. Then it returns the results and it goes to wait for another request from a client.

How the RPCClient object is implemented

The RPCClient object implements the following interface stored in RPC.h:
class RPCClient { 
public:
   RPCClient( char * sharedFileName ); 
   int call( char * rpcName, RPCArgument argument, RPCResult result ); 
};
RPCClient() is the constructor of this object. It opens the file sharedFileName and it maps it into memory using mmap(). If there is an error, it prints the cause of the error and exits.
call() calls a remote procedure call. rpcName is the name of the procedure that will be invoked. argument and result are two buffers where the arguments and results are stored. See RPC.h for the maximum size of these buffers. call() will block until the remote call is serviced by the server, and then it will return the results in result. If any error happens, call() will return a value different than 0. 
 

How to do the synchronization between clients and servers

A client will block until a call is serviced by the server, and a server will block until a call arrives. To do this synchronization you will use semaphores. See the manual pages of sema_init, sema_wait, and sema_post. This semaphores will be in shared memory.
Also your implementation must support multiple simmultaneous calls in the server. To implement this, RPCShared has multiple RPCThread objects in _rpcThreads, one for every simultaneous call. In the server there is going to be one thread for every RPCThread waiting for invocations. A client that makes a call will have to wait until one RPCThread is available. The structure of RPCThread is in RPC.h.

How to start

Your first goal is to make the program client1.cc and server1.cc run. Implement first this mechanism supporting only one call and with no threads in the server.

Testing with a Multithreaded Client and Server


In this part of the project you will test  your implementation using a multithreaded client. 
Step 1.

In RPCServer::runServer() create  _maxAvailableThreads -1 threads that call the RPCServer::threadServer() method using thread_create(). Then have the runServer procedure call threadServer() so the main thread will become also one of the server threads.
RPCServer::runServer() 
{
    for ( int i = 0; i <= _shared->_maxAvailableThreads-1; i++)
    { 
      thr_create( 0, 0, threadServer, this, THR_BOUND, 0 ); 
    } 
    threadServer( this ); 
}
Step 2.

You will add three new fields to the RPCShared class: _currentSimultaneousCalls, _maxSimultaneousCalls, and _totalCalls. _currentSimultaneousCalls stores the number of  simultaneous calls that are being perfomed in the server at any particular moment in time. _maxSimultaneousCalls stores the maximum value of _currentSimultaneousCalls. _totalCalls is the total number of calls that have been performed in the server. You will add code to RPCClient::call() to update these three fields whenever a call is made. To have access to the values of these fields, you will add two methods to the RPCClient class. The first procedure is called RPCClient::maxSimultaneousCalls() that returns the value of _maxSimultaneousCalls. The second procedure is called  RPCClient::totalCalls() that returns the value of  _totalCalls. The file RPC.h contains the definitions of these new three fields and these two new methods.
Step 3.

There is a test program to test your RPC implementation. This new program implements a simple Bank application and uses inter-process RPC to communicate the client and the server. The files that implement this application are in BankClient.cc,BankServer.cc and Bank.h. The Makefile contains instructions to build it. To run the BankServer in the background type:
BankServer none &
The argument none means that the BankServer will run without synchronization at all. Other arguments that can be used instead of none are global and account. They  are explained later this document.
To execute the 4 different tests in the BankClient type:

BankClient 1 
BankClient 2 
BankClient 3
BankClient 4
Test 1 creates a new account in the bank and gets information about the new account. Test 2 creates 10 accounts, performs 100000 random transfers between accounts, and prints the total funds at the end. Test 3 and 4 do the same test 2 does, however test3 creates 2 threads, and test 4 creates 10 threads. If your RPC implementation is correct, you will be able to execute successfully tests 1 and 2. Test 3 and 4 will fail because BankServer does not have any synchronization mechanisms to prevent race conditions. 
Step 4.

You will implement two new synchronization mechanisms in the BankServer to prevent race conditions and to be able to run test 3 and 4 successfully. The first mechanism is called Global-Mutex and the second mechanism is called Account-Mutex. The synchronization mechanism used is selected by passing it as argument to BankServer. When none is passed as argument, the BankServer will run with no synchronization mechanism. When global is passed as argument, the BankServer will use the  Global-Mutex mechanism for synchronization. When account is passed as rgument, the BankServer will use the Account-Mutex mechanism instead.
The Global-Mutex mechanism consists of a having a single mutex emaphore for the entire bank to serialize the bank operations. You will insert the necessary semaphore calls in the BankServer implementation to enforce mutual exclusion in the Bank calls and to prevent race conditions. Test your implementation using test 3 and test 4.

The Account-Mutex synchronization mechanism consists of one mutex semaphore for each account in the bank, i.e., the number of mutex semaphores will be the same as the number of accounts in the Bank. This increases the parallelism in the bank because several calls can be executed simultaneously inside the bank as long as they are executed in different accounts. Make sure that no dead-locks occur and test your implementation running test 3 and 4.

To use the Global-Mutex mechanism type:

    BankServer global &

To use the Account-Mutex mechanism type:

    BankServer account &

Try to understand how BankClient and BankServer are implemented before you start doing any modification. The sources are in BankClient.cc,BankServer.cc,Bank.h, and Makefile. You can get all the sources from src.

Deadline and How to turnin 
The deadline of the project is Sunday October 28th, 2012 at 11:59pm.

Make sure that the tests run and type the following in lore toturn-in your project.

turnin -c cs354 -p lab3 lab3-src 
