SRCS := $(wildcard *.c)
OBJS := $(patsubst %c,%o,$(SRCS))
all:${OBJS}
%.o:%.c
	@echo -e "\e[32;1m[CC]\e[0m\t" $<
	@$(CC) ${CFLAGS} -mgeneral-regs-only -c $< -o ${BASE_DIR}/build/$@
