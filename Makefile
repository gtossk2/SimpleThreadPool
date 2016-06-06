all: thread_pool.o 
	gcc main.c thread_pool.o -lpthread -o example

thread_pool.o: thread_pool.c 
	gcc thread_pool.c -c

clean:
	rm -rf example *.o
