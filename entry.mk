all:build/helloworld.o
build/helloworld.o:
	$(CC) ${CFLAGS} -c helloworld.c -o build/helloworld.o