ifneq '$(USER)' 'fhh'
GNUEFI_PATH=/usr/include/efi
else
GNUEFI_PATH=/usr/local/include/efi
endif
BASE_DIR=${PWD}
CFLAGS=-isystem "${PWD}/include" -isystem "${PWD}/drivers/include"  -isystem "${PWD}/common/include" -isystem "${PWD}/memory/include" -isystem "${PWD}/acpi/include" -isystem "${GNUEFI_PATH}" -isystem "${GNUEFI_PATH}/x86_64" -Wall -Werror -O2 -fno-stack-protector -Wno-address-of-packed-member
SUBDIRS=drivers memory acpi common
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
install:
	sudo qemu-nbd -c /dev/nbd0 '/home/fhh/VirtualBox VMs/UEFITest/raw.vdi'
	sudo mount -t auto /dev/nbd0p1 /media
	sudo cp -f build/kernel /media/
	sudo umount /media
	sudo qemu-nbd --disconnect /dev/nbd0
