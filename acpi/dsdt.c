#include <dsdt.h>
#include <aml_automata.h>
#include <printk.h>
//看样子需要一个有点复杂的自动机完成这项工作了。
//为了简化事情，我们分为两个步骤处理：
//在初始化阶段，我们构建一棵“树”为dsdt构建索引；
//在运行阶段，我们通过检索“树”来获得对应的数据结点并处理数据。
tree* dsdt_tree;

int parse_dsdt(void* in)
{
    if(validate_table((U8*)in)) {
        printk(" Error:broken dsdt.\n");
        return -1;
    }
    //一项最短的ACPI需要4字节（名称），而一个结点有sizeof(tree_node)个字节。在这里，我们为它们分配buffer。
    U64 buffer_size = (((ACPISDTHeader*)in)->Length/4)*sizeof(tree_node);
    debug("We are going to allocate %d bytes for our tree.\n",buffer_size);
    aml_pass_1(in+sizeof(ACPISDTHeader),dsdt_tree);
    return 0;
}