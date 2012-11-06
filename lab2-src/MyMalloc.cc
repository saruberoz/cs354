//
// CS354: MyMalloc Project
//
// The current implementation gets memory from the OS
// every time memory is requested and never frees memory.
//
// You will implement the allocator as indicated in the handout.
// 
// Also you will need to add the necessary locking mechanisms to
// support multi-threaded programs.
//

//Modified By : Wilson Sumanang

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h> // for checkHeap function

enum {
  ObjFree = 0,
  ObjAllocated = 1
};

// Header of an object. Used both when the object is allocated and freed
class ObjectHeader {
 public:
  int _flags;		      // flags == ObjFree or flags = ObjAllocated
  size_t _objectSize;         // Size of the object. Used both when allocated
			      // and freed.
};

// for checking splitting case
enum {
  found = 0,
  notFound = 1
};

// free list chain
class LinkedList {
 public:
  ObjectHeader *_prev;
  ObjectHeader *_next;
};

// memory address chain
class ListNode {
public :
  ObjectHeader * _objectHeader;
  ListNode * _next;
};

class Allocator {
  // State of the allocator

  // Size of the heap
  size_t _heapSize;

  // True if heap has been initialized
  int _initialized;

  // Verbose mode
  int _verbose;

  // # malloc calls
  int _mallocCalls;

  // # free calls
  int _freeCalls;

  // # realloc calls
  int _reallocCalls;
  
  // # realloc calls
  int _callocCalls;
  
  // MORE DECLARATIONS
  // # Free List Declaration
  ObjectHeader * freeList[65];
  
  // #Lock declaration
  pthread_mutex_t lock;
  
  ListNode objectList[200];

  
public:
  // This is the only instance of the allocator.
  static Allocator TheAllocator;

  //Initializes the heap
  void initialize();

  // Allocates an object 
  void * allocateObject( size_t size );

  // Frees an object
  void freeObject( void * ptr );

  // Returns the size of an object
  size_t objectSize( void * ptr );

  // At exit handler
  void atExitHandler();

  //Prints the heap size and other information about the allocator
  void print();

  // Gets memory from the OS
  void * getMemoryFromOS( size_t size );

  void increaseMallocCalls() { _mallocCalls++; }

  void increaseReallocCalls() { _reallocCalls++; }

  void increaseCallocCalls() { _callocCalls++; }

  void increaseFreeCalls() { _freeCalls++; }
  
  //ADD FUNCTION for doubly linked list
  // Print heap content
  void printHeap();
  
  // insert free memory to free list
  void insertNode(int index, ObjectHeader * mem);
  
  // delete memory from free list
  void deleteNode(ObjectHeader * current, int index);
  
};

Allocator Allocator::TheAllocator;

extern "C" void
atExitHandlerInC()
{
  Allocator::TheAllocator.atExitHandler();
}

void
Allocator::initialize()
{
  // Environment var VERBOSE prints stats at end and turns on debugging
  // Default is on
  _verbose = 1;
  const char * envverbose = getenv( "MALLOCVERBOSE" );
  if ( envverbose && !strcmp( envverbose, "NO") ) {
    _verbose = 0;
  }

  // Initialize free list
  int i;
  for(i = 0 ; i < 65 ; i++){
    freeList[i] = NULL;
  }
  
  // Initialize mutex lock
  pthread_mutex_init(&lock, NULL);
  
  // In verbose mode register also printing statistics at exit
  atexit( atExitHandlerInC );

  _initialized = 1;
}

void *
Allocator::allocateObject( size_t size )
{
  // Simple implementation

  //Make sure that allocator is initialized
  if ( !_initialized ) {
    initialize();
  }
  
  ObjectHeader * o;
  
  // Mutex lock
  pthread_mutex_lock(&lock);
  
  // Add the ObjectHeader to the size and round the total size up to a
  // multiple of 8 bytes for alignment.
  size_t totalSize = (size + 2 * sizeof(ObjectHeader) + 7) & ~7;

  // get the index of the array (from project spec in website)
  int index = totalSize >= 512 ? 64 : ((totalSize) >> 3);

  // Object Allocation
  while(index < 65){
		if(freeList[index] != NULL){
			// check need split - get the size
			ObjectHeader * current = freeList[index];
			size_t currentSize = current->_objectSize;
			int status = found;
	
			if(index == 64){
				// walk on the list
				LinkedList * ln = (LinkedList *) (current + 1); // 1 is sizeof(ObjectHeader) 
																												  					// because the head is of type (ObjectHeader *)
				while(totalSize > currentSize && current != NULL){
					current = ln->_next;

					if(current != NULL){
						currentSize = current->_objectSize;
						ln = (LinkedList *) (current + 1);
					}
				}	

				if(current == NULL){
					// Not found
					status = notFound;
					index++;
				}
			}

			//if(index < 64 || (index == 64 && status == found) ){
			if(status == found){
				if(currentSize - totalSize < 24){
					// do not need to split
					// assign - remove from free list
					o = (ObjectHeader *) current;

					// Set object as allocated
					o->_flags = ObjAllocated;

					// write info on the footer
					ObjectHeader * footer = (ObjectHeader *)((int)o + currentSize - sizeof(ObjectHeader));

					// Set object as allocated
					footer->_flags = ObjAllocated;

					// remove from the free list
					deleteNode(current, index);
				}else{
					// need to split
					// assign - remove from free list
					o = (ObjectHeader *) current;

					// Store the totalSize. We will need it in realloc() and in free()
					o->_objectSize = totalSize;

					// Set object as allocated
					o->_flags = ObjAllocated;

					// write info on the footer
					ObjectHeader * footer = (ObjectHeader *)((int)o + totalSize - sizeof(ObjectHeader));

					// Store the totalSize.
					footer->_objectSize = totalSize;

					// Set object as allocated
					footer->_flags = ObjAllocated;

					// remove from the free list
					deleteNode(current, index);

					// put header & footer on remaining memory chunk
					int remaining = currentSize - totalSize;
					// printf("Remaining: %d\n", remaining);

					// Header for remaining memory
					ObjectHeader * remHeader = (ObjectHeader *)((int)o + totalSize);
					remHeader->_objectSize = remaining;
					remHeader->_flags = ObjFree;

					// Footer for remaining memory
					ObjectHeader * remFooter = (ObjectHeader *)((int)remHeader + remaining - sizeof(ObjectHeader));
					remFooter->_objectSize = remaining;
					remFooter->_flags = ObjFree;

					// insert the remaining
					int insertIndex = remaining >= 512 ? 64:((remaining)>>3);
					insertNode(insertIndex, (ObjectHeader *) ((int)o + totalSize));
				}
			}
			break;
		}

		// increase the iterator
		index++;
	}

  // You should get memory from the OS only if the memory in the free list could not
  // satisfy the request.
  // not found; request from OS - if smaller than 16KB, request 16KB
  if(index >= 65){
		unsigned long sixteenKB = 16 * 1024;
		if(totalSize < (sixteenKB - 2 * sizeof(ObjectHeader))){
			int requestedSize = sixteenKB - 2 * sizeof(ObjectHeader);
			int remaining = requestedSize - totalSize;
			int dummySize = sixteenKB;
			void * mem = getMemoryFromOS( dummySize );
			
			// create Dummy Header & Footer
			ObjectHeader * dummyHeader = (ObjectHeader *) mem;

			// Store the dummySize.
			dummyHeader->_objectSize = dummySize;

			// Set object as allocated - as fence
			dummyHeader->_flags = ObjAllocated;

			// write info on the footer
			ObjectHeader * dummyFooter = (ObjectHeader *)((int)dummyHeader + dummySize - sizeof(ObjectHeader));

			// Store the dummySize.
			dummyFooter->_objectSize = dummySize;

			// Set object as allocated - as fence
			dummyFooter->_flags = ObjAllocated;

			if(remaining < 24){
				// do not split
				// Get a pointer to the object header
				o = (ObjectHeader *) ((int)dummyHeader + sizeof(ObjectHeader));

				// Store the totalSize. We will need it in realloc() and in free()
				o->_objectSize = requestedSize;

				// Set object as allocated
				o->_flags = ObjAllocated;

				// write info on the footer
				ObjectHeader * footer = (ObjectHeader *)((int)o + requestedSize - sizeof(ObjectHeader));

				// Store the totalSize.
				footer->_objectSize = requestedSize;

				// Set object as allocated
				footer->_flags = ObjAllocated;
			}else{
				// need to split
				// Get a pointer to the object header
				o = (ObjectHeader *) ((int)dummyHeader + sizeof(ObjectHeader));

				// Store the totalSize. We will need it in realloc() and in free()
				o->_objectSize = totalSize;

				// Set object as allocated
				o->_flags = ObjAllocated;

				// write info on the footer
				ObjectHeader * footer = (ObjectHeader *)((int)o + totalSize - sizeof(ObjectHeader));

				// Store the totalSize.
				footer->_objectSize = totalSize;

				// Set object as allocated
				footer->_flags = ObjAllocated;

				// put header & footer on remaining memory chunk
				// Header for remaining memory
				ObjectHeader * remHeader = (ObjectHeader *)((int)o + totalSize);
				remHeader->_objectSize = remaining;
				remHeader->_flags = ObjFree;

				// Footer for remaining memory
				ObjectHeader * remFooter = (ObjectHeader *)((int)remHeader + remaining - sizeof(ObjectHeader));
				remFooter->_objectSize = remaining;
				remFooter->_flags = ObjFree;

				// insert the remaining
				int insertIndex = remaining >= 512 ? 64:((remaining)>>3);				
				insertNode(insertIndex, (ObjectHeader *) ((int)o + totalSize));
			}			
		}else{
			//int dummySize = totalSize + 2 * sizeof(ObjectHeader);
			int dummySize = totalSize + 2 * sizeof(ObjectHeader);
			void * mem = getMemoryFromOS( dummySize );
			
			// create Header & Footer
			ObjectHeader * dummyHeader = (ObjectHeader *) mem;

			// Store the totalSize. We will need it in realloc() and in free()
			dummyHeader->_objectSize = dummySize;

			// Set object as allocated
			dummyHeader->_flags = ObjAllocated;

			// write info on the footer
			ObjectHeader * dummyFooter = (ObjectHeader *)((int)dummyHeader + dummySize - sizeof(ObjectHeader));

			// Store the totalSize.
			dummyFooter->_objectSize = dummySize;

			// Set object as allocated
			dummyFooter->_flags = ObjAllocated;

			// Get a pointer to the object header
			o = (ObjectHeader *) ((int)dummyHeader + sizeof(ObjectHeader));

			// Store the totalSize. We will need it in realloc() and in free()
			o->_objectSize = totalSize;

			// Set object as allocated
			o->_flags = ObjAllocated;

			// write info on the footer
			ObjectHeader * footer = (ObjectHeader *)((int)o + totalSize - sizeof(ObjectHeader));

			// Store the totalSize.
			footer->_objectSize = totalSize;

			// Set object as allocated
			footer->_flags = ObjAllocated;
		}
	}
  
  // unlock
  pthread_mutex_unlock(&lock);
  
  // Return the pointer after the object header.
  return (void *) (o + 1);
}

void
Allocator::freeObject( void * ptr )
{
  // Here you will return the object to the free list sorted by address and you will coalesce it
  // if possible.

  // lock your mutex here
  pthread_mutex_lock(&lock);

  // Here you will return the object to the free list sorted by address and you will coalesce it if possible.
  // Step 1&2, get the header and footer and set the flags
  ObjectHeader * head = (ObjectHeader*) (ptr);
  head = (head - 1);

  // check if the block is allocated
  if(head->_flags != ObjAllocated){
	// if not allocated just return
	pthread_mutex_unlock(&lock);
	return;
  }

  // Set object as free
  head->_flags = ObjFree;

  // Set flag on footeer as free
  ObjectHeader * footer = (ObjectHeader *)((int)head + head->_objectSize - sizeof(ObjectHeader));
  footer->_flags = ObjFree;

  // Step3, implement your coalescing here
  ObjectHeader * prevFooter = (ObjectHeader*)((int)head - sizeof(ObjectHeader));
  ObjectHeader * nextHeader = (ObjectHeader*)((int)footer + sizeof(ObjectHeader));
  ObjectHeader * currentHeader = head;
  ObjectHeader * currentFooter = footer;

  if(prevFooter->_flags != ObjFree && nextHeader->_flags != ObjFree){
	// no need to coales and insert back to free list
	int insertIndex = head->_objectSize >= 512 ? 64:((head->_objectSize)>>3);
	insertNode(insertIndex, head);
  }else{
	// coales them
	size_t newSize = 0;
	bool coalesWithPrev = false;

	if(prevFooter->_flags == ObjFree){
		// get previous size
		size_t prevSize = prevFooter->_objectSize;
		newSize = newSize + prevSize + head->_objectSize;

		// set the previous head pointer
		ObjectHeader * prevHeader = (ObjectHeader *)((int)prevFooter - prevSize + sizeof(ObjectHeader));

		// delete from free list
		int index = prevSize >= 512 ? 64:((prevSize)>>3);
		deleteNode(prevHeader, index);

		// delete reference & size
		head->_flags = NULL;
		head->_objectSize = NULL;
		prevFooter->_flags = NULL;
		prevFooter->_objectSize = NULL;

		// update head pointer
		head = prevHeader;

		// update the size
		head->_objectSize = newSize;
		footer->_objectSize = newSize;

		// check if already coales with previous block
		coalesWithPrev = true;
	}

	if(nextHeader->_flags == ObjFree){
		// get previous size
		size_t nextSize = nextHeader->_objectSize;
		
		if(coalesWithPrev == true){
			newSize = newSize + nextSize;
		}else{
			newSize = newSize + nextSize + head->_objectSize;
		}

		// set the next footer pointer
		ObjectHeader * nextFooter = (ObjectHeader *)((int)nextHeader + nextSize - sizeof(ObjectHeader));

		// delete from free list
		int index = nextSize >= 512 ? 64:((nextSize)>>3);
		deleteNode(nextHeader, index);

		// delete reference & size
		footer->_flags = NULL;
		footer->_objectSize = NULL;
		nextHeader->_flags = NULL;
		nextHeader->_objectSize = NULL;

		// update footer pointer
		footer = nextFooter;

		// update the size
		head->_objectSize = newSize;
		footer->_objectSize = newSize;
	}

	int index = newSize >= 512 ? 64:((newSize)>>3);
	insertNode(index, head);
  }

  // unlock your mutex here
  pthread_mutex_unlock(&lock);
	
  // Simple allocator does nothing.
	
}

size_t
Allocator::objectSize( void * ptr )
{
  // Return the size of the object pointed by ptr. We assume that ptr is a valid obejct.
  ObjectHeader * o =
    (ObjectHeader *) ( (char *) ptr - sizeof(ObjectHeader) );

  // Substract the size of the header
  return o->_objectSize - sizeof(ObjectHeader);
}

void
Allocator::print()
{
  printf("\n-------------------\n");

  printf("HeapSize:\t%d bytes\n", _heapSize );
  printf("# mallocs:\t%d\n", _mallocCalls );
  printf("# reallocs:\t%d\n", _reallocCalls );
  printf("# callocs:\t%d\n", _callocCalls );
  printf("# frees:\t%d\n", _freeCalls );

  printf("\n-------------------\n");
}

void *
Allocator::getMemoryFromOS( size_t size )
{
  // Use sbrk() to get memory from OS
  _heapSize += size;
  
  return sbrk( size );
}

void
Allocator::atExitHandler()
{
  // Print statistics when exit
  if ( _verbose ) {
    print();
  }
}

//
// C interface
//

extern "C" void *
malloc(size_t size)
{
  Allocator::TheAllocator.increaseMallocCalls();
  
  return Allocator::TheAllocator.allocateObject( size );
}

extern "C" void
free(void *ptr)
{
  Allocator::TheAllocator.increaseFreeCalls();
  
  if ( ptr == 0 ) {
    // No object to free
    return;
  }
  
  Allocator::TheAllocator.freeObject( ptr );
}

extern "C" void *
realloc(void *ptr, size_t size)
{
  Allocator::TheAllocator.increaseReallocCalls();
    
  // Allocate new object
  void * newptr = Allocator::TheAllocator.allocateObject( size );

  // Copy old object only if ptr != 0
  if ( ptr != 0 ) {
    
    // copy only the minimum number of bytes
    size_t sizeToCopy =  Allocator::TheAllocator.objectSize( ptr );
    if ( sizeToCopy > size ) {
      sizeToCopy = size;
    }
    
    memcpy( newptr, ptr, sizeToCopy );

    //Free old object
    Allocator::TheAllocator.freeObject( ptr );
  }

  return newptr;
}

extern "C" void *
calloc(size_t nelem, size_t elsize)
{
  Allocator::TheAllocator.increaseCallocCalls();
    
  // calloc allocates and initializes
  size_t size = nelem * elsize;

  void * ptr = Allocator::TheAllocator.allocateObject( size );

  if ( ptr ) {
    // No error
    // Initialize chunk with 0s
    memset( ptr, 0, size );
  }

  return ptr;
}

extern "C" void 
checkHeap()
{
	// Verifies the heap consistency by iterating over all objects
	// in the free lists and checking that the next, previous pointers
	// size, and boundary tags make sense.
	// The checks are done by calling assert( expr ), where "expr"
	// is a condition that should be always true for an object.
	//
	// assert will print the file and line number and abort
	// if the expression "expr" is false.
	//
	// checkHeap() is required for your project and also it will be 
	// useful for debugging.
}

void
Allocator::insertNode( int insertIndex, ObjectHeader * mem )
{
	if(freeList[insertIndex] == NULL){
		// the list is empty
		freeList[insertIndex] = mem;

		// move the head to the header of the block
		LinkedList * ln = (LinkedList *) (mem + 1); // +1 here is in fact +sizeof(ObjectHeader) because head is type of ObjectHeader

		ln->_prev = NULL;
		ln->_next = NULL;
	}else{
		// the list is not empty - insert in sorted order (based on the size)
		ObjectHeader * currentNode = freeList[insertIndex];

		// get the size of the new object and the current object
		size_t newObjectSize = mem->_objectSize;
		size_t currentObjectSize = currentNode->_objectSize;

		// walk on the list
		LinkedList * ln = (LinkedList *) (currentNode + 1); // +1 here is in fact +sizeof(ObjectHeader) 
																												  			// the head is of type (ObjectHeader *)
		while(newObjectSize > currentObjectSize && ln->_next != NULL){
			currentNode = ln->_next;
			currentObjectSize = currentNode->_objectSize;
			ln = (LinkedList *) (currentNode + 1);
		}

		if(ln->_prev == NULL){
			// insert to head
			freeList[insertIndex] = mem;
			ln = (LinkedList *) (mem + 1); // +1 here is in fact +sizeof(ObjectHeader) 
																				 
			ln->_prev = NULL;
			ln->_next = currentNode;

			// update the provious pointer
			ln = (LinkedList *) (currentNode + 1);
			ln->_prev = mem;

		}else if(ln->_next == NULL && newObjectSize > currentObjectSize){
			// insert to tail
			ln->_next = mem;

			ln = (LinkedList *) (mem + 1);
			ln->_prev = currentNode;
			ln->_next = NULL;
		}else{
			// insert in the middle
			ObjectHeader * previousNode = ln->_prev;
			ln->_prev = mem;
			
			ln = (LinkedList *) (previousNode + 1);
			ln->_next = mem;

			ln = (LinkedList *) (mem + 1);
			ln->_prev = previousNode;
			ln->_next = currentNode;
		}
	}

	return;
} // insertNode

void 
Allocator::deleteNode( ObjectHeader * current, int deleteIndex )
{
	// move the head to the header of the block
	LinkedList * ln = (LinkedList *) (current + 1); // +1 here is in fact +sizeof(ObjectHeader)

	if(ln->_next == NULL && ln->_prev == NULL){
		// the only object in the list
		freeList[deleteIndex] = NULL;
		
	}else{
		if(ln->_prev == NULL){
			// the first node
			freeList[deleteIndex] = ln->_next;

			// update pointer
			ln = (LinkedList *) (freeList[deleteIndex] + 1);
			ln->_prev = NULL;
			
		}else if(ln->_next == NULL){
			// the last node
			ObjectHeader * previousNode = ln->_prev;

			// update pointer
			
			ln = (LinkedList *) (previousNode + 1);
			ln->_next = NULL;
		}else{
			// the middle node
			ObjectHeader * previousNode = ln->_prev;
			ObjectHeader * nextNode = ln->_next;
			
			// update pointer
			ln = (LinkedList *) (nextNode + 1);
			ln->_prev = previousNode;

			ln = (LinkedList *) (previousNode + 1);
			ln->_next = nextNode;
		}
	}

	return;
} // deleteNode


void 
Allocator::printHeap()
{
	// print all the free objects in the free lists in the address order
	int i = 0;
	for(i = 0; i < 200; i++)
	{
		objectList[i]._objectHeader = 0;
		objectList[i]._next = 0;
	}

	int array_index = 0;

	objectList[array_index]._objectHeader = NULL; // set the memory object list header
	objectList[array_index]._next = NULL;
	array_index++;

	// insert all the objects up to 200 numbers into the objectList array
	// and chain them in the address order
	for(i = 0; i < 65; i++)
	{
		ObjectHeader * list_obj = (ObjectHeader *) freeList[i];
		while(list_obj)
		{
			objectList[array_index]._objectHeader = list_obj;
			ListNode * all_obj = &objectList[0];
			ListNode * all_obj_prev = NULL;
			while(all_obj)
			{
				if(all_obj->_objectHeader < list_obj)
				{
					all_obj_prev = all_obj;
					all_obj = all_obj->_next;
				}	
				else 
					break;
			}
			all_obj_prev->_next = &objectList[array_index];
			objectList[array_index]._next = all_obj;

			array_index++;
			if(array_index == 200)
			{
				break;
			}

			list_obj = ((LinkedList *)(list_obj+1))->_next;
		}

		if(array_index == 200)
		{
			break;
		}
	}

	ListNode * list_head = &objectList[0];

	// print the list which is in address order
	printf("\nxxxxxxxxxxxxxxx\n");
	while(list_head)
	{
		if(list_head->_objectHeader)
		{
			printf("----------\n");
			printf("Header %x\n", list_head->_objectHeader);
			printf("objectSize %d ", list_head->_objectHeader->_objectSize);
			printf("flags %d\n", list_head->_objectHeader->_flags);
			LinkedList * link_node = (LinkedList *)(list_head->_objectHeader + 1);
			printf("_prev %x ", link_node->_prev);
			printf("_next %x\n", link_node->_next);
			printf("blabla...\n");
			ObjectHeader * footer = (ObjectHeader *)((int)list_head->_objectHeader + list_head->_objectHeader->_objectSize - sizeof(ObjectHeader));
			printf("Footer %x\n", footer);
			printf("objectSize %d ", footer->_objectSize);
			printf("flags %d\n", footer->_flags);
			printf("----------\n");
		}
		list_head = list_head->_next;
	}

	printf("\nxxxxxxxxxxxxxxx\n\n\n\n\n\n\n");
} // printHeap
