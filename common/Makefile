all:build/kstring.o build/printk.o build/cpuid.o build/linkedlist.o build/halt.o
build/kstring.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c kstring.c -o ${BASE_DIR}/build/kstring.o
build/printk.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c printk.c -o ${BASE_DIR}/build/printk.o
build/cpuid.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c cpuid.c -o ${BASE_DIR}/build/cpuid.o
build/linkedlist.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c linkedlist.c -o ${BASE_DIR}/build/linkedlist.o
build/halt.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c halt.c -o ${BASE_DIR}/build/halt.o