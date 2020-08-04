SRCS := $(wildcard *.c)
OBJS := $(patsubst %c,%o,$(SRCS))
all:${OBJS} assembly
%.o:%.c
	@echo -e "\e[32;1m[CC]\e[0m\t" $<
	@$(CC) ${CFLAGS} -c $< -o ${BASE_DIR}/build/$@
assembly:gdt.s
	@echo -e "\e[32;1m[AS]\e[0m\t" $<
	as $< -o ${BASE_DIR}/build/gdt_s.o 