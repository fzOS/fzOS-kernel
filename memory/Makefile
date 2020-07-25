all:build/memorysetup.o build/memory.o
build/memorysetup.o:
	@echo -e "\e[32;1m[CC]\e[0m" $@
	@$(CC) ${CFLAGS} -c memorysetup.c -o ${BASE_DIR}/build/memorysetup.o
build/memory.o:
	@echo -e "\e[32;1m[CC]\e[0m" $@
	@$(CC) ${CFLAGS} -c memory.c -o ${BASE_DIR}/build/memory.o