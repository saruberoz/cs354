#test1, 2, 3, 5
CC = g++ -g

#test4
#CC = /opt/SUNWspro/bin/CC -g

all: test1 test2 test3 MyMalloc.so runtests

runtests: runtest1-first runtest2-first runtest3-first runtest4-first \
	runtest1-best runtest2-best runtest3-best runtest4-best

clearcounter:
	rm -f npassed
	rm -f nfailed

MyMalloc.so: MyMalloc.cc
	$(CC) -c -g MyMalloc.cc
	ld -G -o MyMalloc.so MyMalloc.o

test1: test1.cc MyMalloc.so
	$(CC) -o test1 test1.cc MyMalloc.so

test2: test2.cc MyMalloc.so
	$(CC) -o test2 test2.cc MyMalloc.so

test3: test3.cc MyMalloc.so
	$(CC) -o test3 test3.cc MyMalloc.so -lthread

runtest1-first:
	MALLOCPOLICY=FIRSTFIT && export MALLOCPOLICY && \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test1 MALLOCPOLICY=FIRSTFIT ---" && \
	test1

runtest2-first:
	MALLOCPOLICY=FIRSTFIT && export MALLOCPOLICY && \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test2 MALLOCPOLICY=FIRSTFIT ---" && \
	test2

runtest3-first:
	MALLOCPOLICY=FIRSTFIT && export MALLOCPOLICY && \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test3 MALLOCPOLICY=FIRSTFIT ---" && \
	test3

runtest4-first: MyMalloc.so
	MALLOCPOLICY=FIRSTFIT && export MALLOCPOLICY && \
	echo && \
	echo "---- Running test4 (run cc with your malloc) MALLOCPOLICY=FIRSTFIT ----" && \
	LD_PRELOAD=`pwd`/MyMalloc.so && \
	export LD_PRELOAD && \
	/opt/SUNWspro/bin/cc hello.c && \
	#g++ hello.c && \
	echo ">>> Test4 passed\n" || \
	echo "*** Test4 failed\n"

runtest1-best:
	MALLOCPOLICY=BESTFIT && export MALLOCPOLICY && \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test1 MALLOCPOLICY=BESTFIT ---" && \
	test1

runtest2-best:
	MALLOCPOLICY=BESTFIT && export MALLOCPOLICY && \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test2 MALLOCPOLICY=BESTFIT ---" && \
	test2

runtest3-best:
	MALLOCPOLICY=BESTFIT && export MALLOCPOLICY && \
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:`pwd` && export LD_LIBRARY_PATH && \
	echo "--- Running test3 MALLOCPOLICY=BESTFIT ---" && \
	test3

runtest4-best: MyMalloc.so
	MALLOCPOLICY=BESTFIT && export MALLOCPOLICY && \
	echo && \
	echo "---- Running test4 (run cc with your malloc) MALLOCPOLICY=BESTFIT ----" && \
	LD_PRELOAD=`pwd`/MyMalloc.so && \
	export LD_PRELOAD && \
	/opt/SUNWspro/bin/cc hello.c && \
	#g++ hello.c && \
	echo ">>> Test4 passed\n" || \
	echo "*** Test4 failed\n"

clean:
	rm -f *.o test1 test2 test3 test4 test5 MyMalloc.so core a.out
