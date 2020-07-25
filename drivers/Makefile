all:build/fbcon.o build/graphics.o
build/fbcon.o:
	@echo -e "\e[32;1m[CC]\e[0m" $@
	@$(CC) ${CFLAGS} -c fbcon.c -o ${BASE_DIR}/build/fbcon.o
build/graphics.o:
	@echo -e "\e[32;1m[CC]\e[0m" $@
	@$(CC) ${CFLAGS} -c graphics.c -o ${BASE_DIR}/build/graphics.o