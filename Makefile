CC = gcc

all:	sandbox debug
sandbox:	sandbox.c
	$(CC) sandbox.c -o sandbox
debug:	sandbox.c
	$(CC) -g sandbox.c -o sandbox_debug
clean:
	rm -f sandbox
	rm -f sandbox_debug
