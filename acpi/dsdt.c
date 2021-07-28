#include <acpi/dsdt.h>
#include <common/printk.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
//看样子需要一个有点复杂的自动机完成这项工作了。
//为了简化事情，我们分为两个步骤处理：
//在初始化阶段，我们构建一棵“树”为dsdt构建索引；
//在运行阶段，我们通过检索“树”来获得对应的数据结点并处理数据。

int parse_dsdt(void* in)
{
    lai_create_namespace();
    lai_enable_acpi(1);
    (void)in;
    U32 result = lai_enable_acpi(1);
    if(result!=0) {
        printk(" LAI refused to enable ACPI.\n");
    }
    return result;
}
