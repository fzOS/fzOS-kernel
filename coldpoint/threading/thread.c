#include <coldpoint/threading/thread.h>
#include <memory/memory.h>
#include <common/printk.h>
#include <stddef.h>
#include <coldpoint/common/class.h>
#include <coldpoint/automata/automata.h>
#include <common/bswap.h>
static U64 g_current_pid=0;
process* create_process(void)
{
    process* p = memalloc(sizeof(process));
    memset(p,0x00,sizeof(process));
    p->pid = g_current_pid++;
    return p;
}
void destroy_process(process* p)
{
    memfree(p);;
}
void destroy_thread(thread* t)
{
    memfree(t);
}
thread* create_thread(process* p,CodeAttribute* c,class* class)
{
    thread* t = memalloc(sizeof(thread));
    memset(t,0x00,offsetof(thread,stack));
    t->class = class;
    t->code = c;
    t->process = p;
    t->rsp = sizeof(stack_frame)+bswap16(t->code->max_locals)+1;
    return t;
}
void thread_test(class* c)
{
    process* p = create_process();
    U64 code_name_index = class_get_utf8_string_index(c,(U8*)"Code");
    MethodInfoEntry* main = class_get_method_by_name_and_desc(c, class_get_utf8_string_index(c,(U8*)"main"), class_get_utf8_string_index(c,(U8*)"([Ljava/lang/String;)V"));
    if(main==nullptr) {
        printk("Cannot load public static void main(String[] args).");
    }
    CodeAttribute* code = (CodeAttribute*)&c->buffer[class_get_method_attribute_by_name(c,main,code_name_index)->info_offset];
    thread* t = create_thread(p,code,c);
    (void)t;
    automata_main_loop(t);
}
