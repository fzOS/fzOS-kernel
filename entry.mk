all:build/helloworld.o
build/helloworld.o:
	gcc ${CFLAGS} -c helloworld.c -o build/helloworld.o