CC = gcc

all:	sandbox debug
sandbox:	sandbox.c
	$(CC) sandbox.c -o sandbox -lseccomp
debug:	sandbox.c
	$(CC) -g sandbox.c -o debugbox -lseccomp
clean:
	rm -f sandbox
	rm -f debugbox
