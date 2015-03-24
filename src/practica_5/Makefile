all: hilos proc sema

semaphore.o: semaphore.c semaphore.h
	gcc -c semaphore.c

hilos: semaphore.o NarcoTrenes1.c
	gcc -o hilos NarcoTrenes2.c semaphore.o

proc: semaphore.o NarcoTrenes2.c
	gcc -o proc NarcoTrenes2.c semaphore.o

sema: semaphore.o NarcoTrenes3.c
	gcc -o sema NarcoTrenes3.c semaphore.o

clean:
	rm hilos
	rm proc
	rm sema
