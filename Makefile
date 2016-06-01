all:
	gcc main.c -lpthread -o example

clean:
	rm example
