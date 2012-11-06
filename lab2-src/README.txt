Created By : Wilson Sumanang

I implemented malloc as specified by the project description using double linked list, using fencing.
freeing is implemented using coalescing if there is a free neighbour
allocating is implemented by splitting if there is not enough memory, and as specified in prject description

Created a function to print the heap; 	

I created a simple test for multiple permutation of memory allocation in test4.cc

In addition i also create a test4.cc which contains:
    Malloc an object from a free list that is a precise match
    Malloc an object from a free list that needs to be split.
    Free an object that needs to be coalesced with the left neighbor only
    Free an object that needs to be coalesced with the right neighbor only
    Free an object that needs to be coalesced with both right and left neighbors
    Free an object that does not need to be coalesced
    Free an object at the beginning of  the heap to test fence posts.
    Free an object at the end of the heap to test fence posts
    See that memory is being reused
    Free an already freed object should not crash
    Free an object that was not allocated should not crash.
    Other conditions not listed in this list. 


