all:build/acpi_parser.o build/xsdt.o build/apic.o
build/acpi_parser.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c parser.c -o ${BASE_DIR}/build/acpi_parser.o
build/xsdt.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c xsdt.c -o ${BASE_DIR}/build/xsdt.o
build/apic.o:
	@echo -e "\e[32;1m[CC]\e[0m	" $@
	@$(CC) ${CFLAGS} -c apic.c -o ${BASE_DIR}/build/apic.o
