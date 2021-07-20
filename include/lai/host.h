
/*
 * Lightweight ACPI Implementation
 * Copyright (C) 2018-2019 the lai authors
 */

#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

struct lai_variable_t;
typedef struct lai_variable_t lai_variable_t;

#define LAI_DEBUG_LOG 1
#define LAI_WARN_LOG 2

// OS-specific functions.
void *laihost_malloc(size_t);
void *laihost_realloc(void *, size_t);
void laihost_free(void *);

__attribute__((weak)) void laihost_log(int, const char *);
__attribute__((weak, noreturn)) void laihost_panic(const char *);

__attribute__((weak)) void *laihost_scan(char *, size_t);
__attribute__((weak)) void *laihost_map(size_t, size_t);
__attribute__((weak)) void laihost_outb(uint16_t, uint8_t);
__attribute__((weak)) void laihost_outw(uint16_t, uint16_t);
__attribute__((weak)) void laihost_outd(uint16_t, uint32_t);
__attribute__((weak)) uint8_t laihost_inb(uint16_t);
__attribute__((weak)) uint16_t laihost_inw(uint16_t);
__attribute__((weak)) uint32_t laihost_ind(uint16_t);

__attribute__((weak)) void laihost_pci_writeb(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t, uint8_t);
__attribute__((weak)) uint8_t laihost_pci_readb(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t);
__attribute__((weak)) void laihost_pci_writew(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t, uint16_t);
__attribute__((weak)) uint16_t laihost_pci_readw(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t);
__attribute__((weak)) void laihost_pci_writed(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t, uint32_t);
__attribute__((weak)) uint32_t laihost_pci_readd(uint16_t, uint8_t, uint8_t, uint8_t, uint16_t);

__attribute__((weak)) void laihost_sleep(uint64_t);

__attribute__((weak)) void laihost_handle_amldebug(lai_variable_t *);

