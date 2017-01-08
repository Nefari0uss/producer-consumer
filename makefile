# makefile for custom unix shell
all: producer-consumer.c
	gcc producer-consumer.c -o pc -lpthread -lrt -O3 -Wall

clean:
	$(RM) pc

