SRCS := $(wildcard *.c) $(wildcard *.s)
OBJS := $(patsubst %c,%c.o,$(SRCS)) $(patsubst %s,%s.o,$(SRCS))
SUBDIRS=common threading automata heap
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        ( cd $$subdir && $(MAKE) all -f Makefile -e CC="${CC}" -e BASE_DIR=${BASE_DIR} -e CFLAGS='${CFLAGS}') || exit 1; \
        done
all:sub ${OBJS}
sub:
	$(RECURSIVE_MAKE)
%.c.o:%.c
	@echo -e "\e[32;1m[CC]\e[0m\t" $<
	@$(CC) ${CFLAGS} -c $< -o ${BASE_DIR}/build/$@
%.s.o:%.s
	@echo -e "\e[36;1m[AS]\e[0m\t" $<
	@as $< -o ${BASE_DIR}/build/$@
