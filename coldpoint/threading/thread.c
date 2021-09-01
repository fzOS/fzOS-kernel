#include <coldpoint/threading/thread.h>
#include <memory/memory.h>
#include <common/printk.h>
#include <coldpoint/common/class.h>
void thread_test(const class* c)
{
    U64 code_name_index = class_get_utf8_string_index(c,(U8*)"Code");
    thread* main_thread = memalloc(sizeof(thread));
    method_info_entry* main = class_get_method_by_name_and_desc(c, class_get_utf8_string_index(c,(U8*)"main"), class_get_utf8_string_index(c,(U8*)"([Ljava/lang/String;)V"));
    if(main==nullptr) {
        printk("Cannot load public static void main(String[] args).");
    }
    main_thread->pc = 0;
    main_thread->code = (code_attribute*)&c->buffer[class_get_method_attribute_by_name(c,main,code_name_index)->info_offset];
    main_thread->rbp=0;
    main_thread->rsp = sizeof(stack_frame)+main_thread->code->max_locals;
    printk("Now executing %b.\n",main_thread->code->code[main_thread->pc]);
    //压入一个测试变量。
    main_thread->stack[main_thread->rsp].data=0x1234567876543210L;
    printk("Test Var is :%x\n",main_thread->stack[main_thread->rsp].data);
    main_thread->pc++;
    //假设发生了跳转！
    U64 new_stack_begin_offset = main_thread->rbp
                                +sizeof(stack_frame)/sizeof(U64)
                                +main_thread->code->max_locals
                                +main_thread->code->max_stack;
    main_thread->stack[new_stack_begin_offset].data = (U64)main_thread->code;
    main_thread->stack[new_stack_begin_offset].type = STACK_TYPE_CODE_POINTER;
    main_thread->stack[new_stack_begin_offset+1].data = main_thread->pc;
    main_thread->stack[new_stack_begin_offset+1].type = STACK_TYPE_PC;
    main_thread->stack[new_stack_begin_offset+2].data = main_thread->rsp;
    main_thread->stack[new_stack_begin_offset+2].type = STACK_TYPE_RSP;
    main_thread->stack[new_stack_begin_offset+3].data = main_thread->rbp;
    main_thread->stack[new_stack_begin_offset+3].type = STACK_TYPE_RBP;
    //随便压入一个临时变量试试！
    main_thread->stack[new_stack_begin_offset+sizeof(stack_frame)/sizeof(U64)+main_thread->code->max_locals].data  = 1;
    main_thread->stack[new_stack_begin_offset+sizeof(stack_frame)/sizeof(U64)+main_thread->code->max_locals].type = STACK_TYPE_INT;
    //加载第二个方法。

    method_info_entry* area = class_get_method_by_name_and_desc(c, class_get_utf8_string_index(c,(U8*)"<init>"), class_get_utf8_string_index(c,(U8*)"()V"));
    main_thread->pc=0;
    main_thread->code = (code_attribute*)&c->buffer[class_get_method_attribute_by_name(c,area,code_name_index)->info_offset];
    main_thread->rbp = new_stack_begin_offset;
    main_thread->rsp = new_stack_begin_offset+sizeof(stack_frame)/sizeof(U64)+main_thread->code->max_locals;
    //完成跳转！
    printk("Now executing %b,variable #1=%d.\n",main_thread->code->code[main_thread->pc],
                                                    main_thread->stack[main_thread->rsp].data);
    //执行一条代码！
    main_thread->pc++;
    printk("Now executing %b.\n",main_thread->code->code[main_thread->pc]);
    //返回！
    main_thread->code = (code_attribute*)main_thread->stack[main_thread->rbp].data;
    main_thread->pc   = main_thread->stack[main_thread->rbp+1].data;
    main_thread->rsp  = main_thread->stack[main_thread->rbp+2].data;
    main_thread->rbp  = main_thread->stack[main_thread->rbp+3].data;
    printk("Now executing %b.\n",main_thread->code->code[main_thread->pc]);
    printk("Test Var is :%x\n",main_thread->stack[main_thread->rsp].data);
    memfree(main_thread);
}
