all: proc sema msgs

semaphore.o: semaphore.c semaphore.h
	gcc -c semaphore.c

proc: NarcoTrenes1.c
	gcc -o proc NarcoTrenes1.c

sema: semaphore.o NarcoTrenes2.c
	gcc -o sema NarcoTrenes2.c semaphore.o

msgs: NarcoTrenes3.c
	gcc -o msgs NarcoTrenes3.c

clean:
	rm proc
	rm sema
	rm msgs
