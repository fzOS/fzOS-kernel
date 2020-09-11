SRCS := $(wildcard *.c) $(wildcard *.s)
OBJS := $(patsubst %c,%c.o,$(SRCS)) $(patsubst %s,%s.o,$(SRCS))
all:${OBJS}
%.c.o:%.c
	@echo -e "\e[32;1m[CC]\e[0m\t" $<
	@$(CC) ${CFLAGS} -c $< -o ${BASE_DIR}/build/$@
%.s.o:%.s
	@echo -e "\e[36;1m[AS]\e[0m\t" $<
	@as $< -o ${BASE_DIR}/build/$@