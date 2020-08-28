#ifndef DSDT_H
#define DSDT_H
#include <acpi_parser.h>
#include <tree.h>
int parse_dsdt(void* in);
extern tree* dsdt_tree;
#endif