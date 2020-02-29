EDK_PATH = /home/chengyu/Documents/edk2
GNUEFI_PATH=/usr/include/efi
BASE_DIR=${PWD}
CFLAGS=-isystem "${PWD}/include" -isystem "${PWD}/drivers/include" -isystem "${PWD}/memory/include" -isystem "${GNUEFI_PATH}" -isystem "${GNUEFI_PATH}/x86_64" -O2
SUBDIRS=drivers memory
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        echo "making in $$subdir"; \
        ( cd $$subdir && $(MAKE) all -f Makefile -e BASE_DIR=${BASE_DIR} -e CFLAGS='${CFLAGS}') || exit 1; \
        done

kernel:
	$(RECURSIVE_MAKE)
	#ugly.....
	gcc ${CFLAGS} -c helloworld.c -o build/helloworld.o
	ld -e kernel_main build/*.o -o build/kernel
all:kernel
clean:
	rm -rf build/*


	
