#if we are debugging
DEBUG=1

ifneq '$(USER)' 'fhh'
GNUEFI_PATH=/usr/include/efi
else
export PATH:=/home/fhh/.ccache/:${PATH}
GNUEFI_PATH=/usr/local/include/efi
CC= ccache 
endif
ifeq '$(DEBUG)' '1'
VERSION := $(shell echo git-`git rev-parse --short HEAD`)
else
VERSION := 0.1.3
endif
BASE_DIR=${PWD}
CC:=${CC} gcc
CFLAGS=-pie -DVERSION="\"${VERSION}\"" -isystem "${PWD}/include" -isystem "${PWD}/interrupt/include" -isystem "${PWD}/drivers/include"  -isystem "${PWD}/common/include" -isystem "${PWD}/memory/include" -isystem "${PWD}/acpi/include" -isystem "${PWD}/syscall/include"  -isystem "${PWD}/threading/include" -isystem "${GNUEFI_PATH}" -isystem "${GNUEFI_PATH}/x86_64" -Wall -Werror -O2 -march=native -mtune=native -fno-stack-protector -Wno-address-of-packed-member -Wno-implicit-function-declaration
SUBDIRS=drivers memory acpi common syscall interrupt threading
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        ( cd $$subdir && $(MAKE) all -f Makefile -e CC="${CC}" -e BASE_DIR=${BASE_DIR} -e CFLAGS='${CFLAGS}') || exit 1; \
        done
all:version_update kernel
version_update:
	@echo -e "\e[33;1m[Ver.]\e[0m	" ${VERSION}
kernel:
	mkdir -p build/
	$(RECURSIVE_MAKE)
	@echo -e "\e[32;1m[CC]\e[0m	" build/helloworld.o
	@$(CC) ${CFLAGS} -c helloworld.c -o build/helloworld.o
	@echo -e "\e[34;1m[LD]\e[0m	" kernel
	@ld -e kernel_main build/*.o -o build/kernel -pie -no-dynamic-linker
	@echo -e "\e[37;1m[NM]\e[0m	" symbols
	@nm -n build/kernel > build/symbols
	@echo -e "\e[35;1m[STRIP]\e[0m	" kernel
	@strip build/kernel
clean:
	rm -rf build/*
install:
	guestmount -a '/home/fhh/VirtualBox VMs/UEFITest/raw.vdi' -m /dev/sda1 --rw /media
	cp -f build/kernel /media/
	umount /media
	VBoxManage startvm "UEFITest" -E VBOX_GUI_DBG_ENABLED=true #-E VBOX_GUI_DBG_AUTO_SHOW=true
