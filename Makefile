#if we are debugging
DEBUG=1
GNUEFI_PATH=/usr/include/efi
ifneq '$(USER)' 'fhh'

else
export PATH:=/home/fhh/.ccache/:${PATH}
CC= ccache 
endif
ifeq '$(DEBUG)' '1'
VERSION := $(shell echo git-`git rev-parse --short HEAD`)
else
VERSION := 0.2.0
endif
THIS_YEAR := $(shell date +"%Y")
BASE_DIR=${PWD}
CC:=${CC} gcc
CFLAGS=-pie -DVERSION="\"${VERSION}\"" -DTHIS_YEAR="${THIS_YEAR}" -isystem "${PWD}/include" -Wall -Werror -O2 -fno-stack-protector -Wno-address-of-packed-member -Wno-implicit-function-declaration -mno-red-zone -ffreestanding
SUBDIRS=drivers memory acpi common syscall interrupt threading filesystem coldpoint
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
	@echo -e "\e[32;1m[CC]\e[0m	" helloworld.c
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
	@echo -e "\e[33;1m[INSTALL]\e[0mkernel"
	@a=$$(losetup -f) && \
	echo "Mounting at $${a}p1" && \
	losetup -f -P '/home/fhh/VirtualBox VMs/UEFITest/raw.img' && \
	mount -t auto $${a}p1 /media && \
	cp -f build/kernel /media/ && \
	umount /media && \
	losetup -d $$a
	@su - fhh -c 'VBoxManage startvm "UEFITest" -E VBOX_GUI_DBG_ENABLED=true' #-E VBOX_GUI_DBG_AUTO_SHOW=true
stat:
	@a=$$(eval echo $$(wc -l `find acpi/*.c interrupt/ syscall/ drivers/ memory/ common/ threading/ include/ | grep -v "lai" | grep -v "asciifont"` 2>/dev/null |  tail -n 1)) && \
	echo -e "\e[36;1m[LINES]\e[0m\t$$a"
	@b=$$(git log | grep "^Author: " | wc -l) && \
	echo -e "\e[33;1m[COMMIT]\e[0m$$b"
mount:
	@a=$$(losetup -f) && \
	echo "Mounting at $${a}p1" && \
	losetup -f -P '/home/fhh/VirtualBox VMs/UEFITest/raw.img' && \
	mount -t auto $${a}p1 /media
