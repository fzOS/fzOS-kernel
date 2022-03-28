DEBUG=1
GNUEFI_PATH=/usr/include/efi
ifeq '$(USER)' 'fhh'
export PATH:=/home/fhh/.ccache/:${PATH}
CC= ccache 
else
CC= ""
endif
ifeq '$(DEBUG)' '1'
VERSION := $(shell echo git-`git rev-parse --short HEAD`)
else
VERSION := 0.3.0
endif
THIS_YEAR := $(shell date +"%Y")
BASE_DIR=${PWD}
CC:=${CC}gcc
CC_VER := $(shell LANG=C ${CC} --version | head -1 | awk '{printf($$3)}')
CC_VER_MAJOR := $(shell echo ${CC_VER} | awk -F. '{print $$1}')
CFLAGS=-fpie -DVERSION="\"${VERSION}\"" -DTHIS_YEAR="${THIS_YEAR}" -isystem "${PWD}/include" -isystem "/usr/include/efi/x86_64" -Wall -Werror -O2 -fno-stack-protector -Wno-address-of-packed-member -Wno-implicit-function-declaration -mno-red-zone -ffreestanding
SUBDIRS=drivers memory acpi common syscall interrupt filesystem coldpoint zcrystal
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        ( cd $$subdir && $(MAKE) all -f Makefile -e CC="${CC}" -e BASE_DIR=${BASE_DIR} -e CFLAGS='${CFLAGS}') || exit 1; \
        done
all:check_gcc_ver version_update kernel
check_gcc_ver:
	@echo -e "\e[36;1m[GCC]\e[0m	" ${CC_VER}
	@if [ ${CC_VER_MAJOR} -lt 11 ];then echo -e "\e[31;1m[Error]\e[0m\t GCC is too old.11 is required.";exit 1;fi
version_update:
	@echo -e "\e[33;1m[Ver.]\e[0m	" ${VERSION}
kernel:
	@mkdir -p build/
	$(RECURSIVE_MAKE)
	@echo -e "\e[32;1m[CC]\e[0m	" helloworld.c
	@$(CC) ${CFLAGS} -c helloworld.c -o build/helloworld.o
	@echo -e "\e[34;1m[LD]\e[0m	" kernel
	@ld -e kernel_main build/*.o -o build/kernel -pie -no-dynamic-linker -Ttext-segment=0x1000 #跳过中断向量表 --fix qemu loader problem
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
	# @su - fhh -c 'VBoxManage startvm "UEFITest" -E VBOX_GUI_DBG_ENABLED=true' #-E VBOX_GUI_DBG_AUTO_SHOW=true
stat:
	@a=$$(eval echo $$(wc -l `find acpi/*.c interrupt/ syscall/ drivers/ memory/ common/ coldpoint/ include/ | grep -v "lai" | grep -v "asciifont"` 2>/dev/null |  tail -n 1)) && \
	echo -e "\e[36;1m[LINES]\e[0m\t$$a"
	@b=$$(git log | grep "^Author: " | wc -l) && \
	echo -e "\e[33;1m[COMMIT]\e[0m$$b"
mount:
	@a=$$(losetup -f) && \
	echo "Mounting at $${a}p1" && \
	losetup -f -P '/home/fhh/VirtualBox VMs/UEFITest/raw.img' && \
	mount -t auto $${a}p1 /media
installvm:
	@echo "Mounting to /data" && \
	mount /dev/sdb1 /data && \
	echo "Moving kernel into EFI" && \
	mv build/kernel /data && \
	echo "VM kernel setup Success"
