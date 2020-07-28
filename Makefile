ifneq '$(USER)' 'fhh'
GNUEFI_PATH=/usr/include/efi
else
export PATH:=/home/fhh/.ccache/:${PATH}
GNUEFI_PATH=/usr/local/include/efi
CC=ccache 
endif
BASE_DIR=${PWD}
CC:=${CC} gcc
CFLAGS=-isystem "${PWD}/include" -isystem "${PWD}/drivers/include"  -isystem "${PWD}/common/include" -isystem "${PWD}/memory/include" -isystem "${PWD}/acpi/include" -isystem "${GNUEFI_PATH}" -isystem "${GNUEFI_PATH}/x86_64" -Wall -Werror -O2 -march=native -fno-stack-protector -Wno-address-of-packed-member -Wno-unused-command-line-argument -Wno-visibility -Wno-implicit-function-declaration
SUBDIRS=drivers memory acpi common syscall
RECURSIVE_MAKE= @for subdir in $(SUBDIRS); \
        do \
        ( cd $$subdir && $(MAKE) all -f Makefile -e CC="${CC}" -e BASE_DIR=${BASE_DIR} -e CFLAGS='${CFLAGS}') || exit 1; \
        done
all:version_update kernel
version_update:
	@sed -i "s/git-[0-9A-Za-z]\{7\}/git-`git rev-parse --short HEAD`/" common/include/version.h
	@echo -e "\e[33;1m[Vr]\e[0m" `gcc -DGET_VERSION_DIRECTLY ${CFLAGS} common/include/version.h -o /dev/null  2> tmp.txt && cat tmp.txt | awk 'NR==1 {print $$5}' && rm tmp.txt `
kernel:
	$(RECURSIVE_MAKE)
	@echo -e "\e[32;1m[CC]\e[0m" build/helloworld.o
	@$(CC) ${CFLAGS} -c helloworld.c -o build/helloworld.o
	@echo -e "\e[34;1m[LD]\e[0m" kernel
	@ld -e kernel_main build/*.o -o build/kernel
clean:
	rm -rf build/*
install:
	((sudo qemu-nbd -c /dev/nbd0 '/home/fhh/VirtualBox VMs/UEFITest/raw.vdi') \
	&& (sudo mount -t auto /dev/nbd0p1 /media) \
	&& (sudo cp -f build/kernel /media/) \
	&& (sudo umount /media) \
	&& (sudo qemu-nbd --disconnect /dev/nbd0) \
	&& (sudo -u fhh VBoxManage startvm "UEFITest")) \
	||(sudo qemu-nbd -d /dev/nbd0)
