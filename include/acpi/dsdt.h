#ifndef DSDT_H
#define DSDT_H
#include <acpi/acpi_parser.h>
#include <common/tree.h>
int parse_dsdt(void* in);
extern tree* dsdt_tree;
#endif
