CC = gcc

all:	sandbox
sandbox:	sandbox.c
	$(CC) sandbox.c -o sandbox
clean:
	rm -f sandbox
