#include <coldpoint/threading/thread.h>
#include <memory/memory.h>
#include <common/printk.h>
#include <stddef.h>
#include <coldpoint/common/class.h>
#include <coldpoint/automata/automata.h>
#include <common/bswap.h>
#include <common/linkedlist.h>
#include <common/iterator.h>

InlineLinkedList g_thread_list = {
    .tail = &g_thread_list.head
};
ThreadInlineLinkedListNode* g_current_thread = nullptr;
thread* get_next_thread(void)
{
    if(g_current_thread == nullptr) {
        g_current_thread = (ThreadInlineLinkedListNode*)g_thread_list.head.next;
        return &g_current_thread->t;
    }
    ThreadInlineLinkedListNode* prev = g_current_thread;
    g_current_thread = (ThreadInlineLinkedListNode*)g_current_thread->node.next;
    if(g_current_thread==nullptr) {
        g_current_thread = (ThreadInlineLinkedListNode*)g_thread_list.head.next;
    }
    while (g_current_thread->t.status!=THREAD_READY) {
        if(g_current_thread->t.status == THREAD_BLOCKED&&*g_current_thread->t.sem >0) {
            g_current_thread->t.status=THREAD_READY;
            return &g_current_thread->t;
        }
        if((ThreadInlineLinkedListNode*)g_current_thread==prev) {
            //looping!
            if(g_current_thread->t.status==THREAD_READY) {
                return &g_current_thread->t;
            }
            else {
                return nullptr;
            }
        }
        g_current_thread = (ThreadInlineLinkedListNode*)g_current_thread->node.next;
        if(g_current_thread==nullptr) {
            g_current_thread = (ThreadInlineLinkedListNode*)g_thread_list.head.next;
        }
    }
    return &g_current_thread->t;
}
static U64 g_current_pid=0;
static U64 g_current_tid=0;
process* create_process(void)
{
    process* p = memalloc(sizeof(process));
    memset(p,0x00,sizeof(process));
    p->pid = g_current_pid++;
    p->input_buffer.q.bufsize = 128;
    return p;
}
void destroy_process(process* p)
{
    memfree(p);
}
void destroy_thread(thread* t)
{
    memfree(t);
}
thread* create_thread(process* p,CodeAttribute* c,class* class,Console* con)
{
    ThreadInlineLinkedListNode* thread_node = memalloc(sizeof(ThreadInlineLinkedListNode));
    thread* t = &thread_node->t;
    memset(t,0x00,offsetof(thread,stack));
    t->console = con;
    t->class = class;
    t->code = c;
    t->process = p;
    t->rbp = 0;
    t->pc = 0;
    t->rsp = sizeof(stack_frame)/sizeof(StackVar)+bswap16(t->code->max_locals)+1;
    t->status = THREAD_READY;
    t->tid = g_current_tid++;
    insert_existing_inline_node(&g_thread_list,&thread_node->node,-1);
    return t;
}
FzOSResult terminate_thread(thread* t,ThreadExitStatus status)
{
    t->status = THREAD_TERMINATED;
    //TODO:Destroy status.
    return FzOS_SUCCESS;
}
thread* get_thread_by_tid(U64 tid)
{
    ThreadInlineLinkedListNode* current_thread = (ThreadInlineLinkedListNode*)g_thread_list.head.next;
    while(current_thread!=nullptr) {
        if(current_thread->t.tid==tid) {
            return &current_thread->t;
        }
        current_thread = (ThreadInlineLinkedListNode*)current_thread->node.next;
    }
    return nullptr;
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
    create_thread(p,code,c,g_default_console);
    automata_main_loop();
}
