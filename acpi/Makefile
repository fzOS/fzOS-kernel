all:build/acpi_parser.o build/xsdt.o build/apic.o
build/acpi_parser.o:
	gcc ${CFLAGS} -c parser.c -o ${BASE_DIR}/build/acpi_parser.o
build/xsdt.o:
	gcc ${CFLAGS} -c xsdt.c -o ${BASE_DIR}/build/xsdt.o
build/apic.o:
	gcc ${CFLAGS} -c apic.c -o ${BASE_DIR}/build/apic.o
