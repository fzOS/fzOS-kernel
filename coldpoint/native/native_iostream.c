#include <coldpoint/native/native_iostream.h>
#include <coldpoint/common/class.h>
#include <coldpoint/heap/heap.h>
#include <coldpoint/automata/obj_inst.h>
#include <coldpoint/native/native_typewrapper.h>
#include <drivers/console.h>
#include <memory/memory.h>
#include <common/kstring.h>
const char * const g_iostream_class_name  = "fzos/base/IOStream";
cpstatus native_iostream_printf(thread* t);
cpstatus native_iostream_println(thread* t);
cpstatus native_iostream_putchar(thread* t);
cpstatus native_iostream_getchar(thread* t);
cpstatus native_iostream_getnbytes(thread* t);

#pragma GCC diagnostic ignored "-Wunused-function"

static void native_putU64hex(U64 data,Console* c)
{
    U8 tempint;
    U8 temp1,temp2;
    for (int i = 0; i < 8; i++){
        /* code */
        tempint = (data&(0xff00000000000000 >> (8*i)) ) >> (56-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
             c->common.putchar(&c->common,temp1);
        }else{
            temp1 = temp1 + 87;
             c->common.putchar(&c->common,temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
             c->common.putchar(&c->common,temp2);
        }else{
            temp2 = temp2 + 87;
             c->common.putchar(&c->common,temp2);
        }
    }
}

static void native_putU16hex(U16 data,Console* c)
{
    U8 tempint;
    U8 temp1,temp2;
    for (int i = 0; i < 2; i++){
        /* code */
        tempint = (data&(0xff00 >> (8*i)) ) >> (8-8*i);
        temp1 = (tempint&0xf0) >> 4;
        temp2 = tempint&0x0f;
        if (temp1 < 10){
            temp1 = temp1 + 48;
            c->common.putchar(&c->common,temp1);
        }else{
            temp1 = temp1 + 87;
            c->common.putchar(&c->common,temp1);
        }
        if (temp2 < 10){
            temp2 = temp2 + 48;
            c->common.putchar(&c->common,temp2);
        }else{
            temp2 = temp2 + 87;
            c->common.putchar(&c->common,temp2);
        }
    }
}
static void native_putU8hex(U8 data,Console* c)
{
    U8 temp1,temp2;
    temp1 = (data&0xf0) >> 4;
    temp2 = data&0x0f;
    if (temp1 < 10){
        temp1 = temp1 + 48;
        c->common.putchar(&c->common,temp1);
    }else{
        temp1 = temp1 + 87;
        c->common.putchar(&c->common,temp1);
    }
    if (temp2 < 10){
        temp2 = temp2 + 48;
        c->common.putchar(&c->common,temp2);
    }else{
        temp2 = temp2 + 87;
        c->common.putchar(&c->common,temp2);
    }
}



static void native_putstring(char *str,Console* c)
{
    //为什么不递归呢？？？？
    printk(str);
}

static void native_putnum(U64 num,Console* c)
{
    U8 tempint;
    tempint = num % 10 + 48;
    if ((num / 10) != 0){
        native_putnum((num/10),c);
    }
    c->common.putchar(&c->common,(char) tempint);
}

static void native_putguid(GUID guid,Console* c)
{
    //12345678-8765-4321-2333-666666666666
    native_putU16hex((guid.first&0xFFFF0000)>>16,c);
    native_putU16hex(guid.first&0xFFFF,c);
    c->common.putchar(&c->common,'-');
    native_putU16hex(guid.second,c);
    c->common.putchar(&c->common,'-');
    native_putU16hex(guid.third,c);
    c->common.putchar(&c->common,'-');
    for(int i=0;i<2;i++) {
        native_putU8hex(guid.fourth[i],c);
    }
    c->common.putchar(&c->common,'-');
    for(int i=0;i<6;i++) {
        native_putU8hex(guid.fifth[i],c);
    }
}
static int native_println(const char* format,Console* c)
{
    const char* pointer = format;
    while(*pointer!='\0') {
        c->common.putchar(&c->common,*pointer);
    }
    return pointer-format;
}
static int native_printf(const char* format,Array* a,Console* c)
{
#if 0
    int count=0;
    const char* pointer = format;
    int arg_count;
    while(*pointer!='\0')
    {
        if(*pointer=='%')
        {
            count++;
            pointer++;
            switch(*pointer)
            {
//                 case 'c':{c->common.putchar(&c->common,a->value);break;}
//                 case 'd':{putnum(va_arg(arg,U64));break;}
//                 case 'x':{putU64hex(va_arg(arg,U64));break;}
//                 case 'b':{putU8hex(va_arg(arg,int));break;}
//                 case 'w':{putU16hex(va_arg(arg,int));break;}
//                 case 's':{putstring(va_arg(arg,char*));break;}
//                 case 'g':{putguid(va_arg(arg,GUID));break;}
                //我们支持全彩色了！
                //支持的颜色设置格式：
                //%#RRGGBB
                case '#':{
                    //只有屏幕终端显示颜色。
                    if(c->common.putchar!=fbcon_putchar) {
                        pointer+=6;
                        break;
                    }
                    //放入神奇的颜色转换代码！
                    fbcon_putchar(&c->common,COLOR_SWITCH_CHAR);
                    break;
                }
                default:{count--;break;}
            }
        }
        else
        {
            c->common.putchar(&c->common,*pointer);
        }
        pointer++;
    }
    if(*(pointer-1)!=LINE_SEPARATOR) {
       c->common.flush(&c->common);
    }
    return count;
#endif
    return 0;
}



static NativeMethod g_iostream_methods[] = {
    {
        (U8*)"printf",
        (U8*)"(Ljava/lang/String;[Ljava/lang/Object;)V",
        native_iostream_printf
    },
    {
        (U8*)"println",
        (U8*)"(Ljava/lang/String;)V",
        native_iostream_println
    },
    {
        (U8*)"putchar",
        (U8*)"(C)V",
        native_iostream_putchar
    },
    {
        (U8*)"getchar",
        (U8*)"()V",
        native_iostream_getchar
    }

};

cpstatus native_iostream_printf(thread* t)
{
    print_opcode("Got native call printf().\n");
    //(Ljava/lang/String;[Ljava/lang/Object;)V
    //Get String.
    Array* a = (Array*)t->stack[t->rsp].data;
    NativeTypeWrapperObject* o = (NativeTypeWrapperObject*)t->stack[t->rsp-1].data;
    t->rsp-=2;
    const char* val = (const char*)get_constant_from_string(o);
    if(val!=nullptr) {
        native_printf(val,a,t->console);
    }
    else {
        except(t,"No string found");
        return COLD_POINT_EXEC_FAILURE;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus native_iostream_println(thread* t)
{
    print_opcode("Got native call println().\n");
    //(Ljava/lang/String;[Ljava/lang/Object;)V
    //Get String.
    NativeTypeWrapperObject* o = (NativeTypeWrapperObject*)t->stack[t->rsp-1].data;
    t->rsp-=2;
    const char* val = (const char*)get_constant_from_string(o);
    if(val!=nullptr) {
        native_println(val,t->console);
    }
    else {
        except(t,"No string found");
        return COLD_POINT_EXEC_FAILURE;
    }
    return COLD_POINT_SUCCESS;
}
cpstatus native_iostream_putchar(thread* t)
{
    print_opcode("Got native call putchar().\n");
    char c = (U8)t->stack[t->rsp].data;
    t->rsp--;
    t->console->common.putchar(&t->console->common,c);
    return COLD_POINT_SUCCESS;
}
cpstatus native_iostream_getchar(thread* t)
{
    print_opcode("Got native call getchar().\n");
    char c = t->console->common.getchar(&t->console->common);
    t->stack[++t->rsp].data = c;
    t->stack[t->rsp].type   = STACK_TYPE_CHAR;
    return COLD_POINT_SUCCESS;
}
static NativeClassInlineLinkedListNode g_iostream_class_linked_node = {
    .c.class_name = (const U8*)g_iostream_class_name,
    .c.type = CLASS_KERNEL_API,
    .c.entry = iostream_class_entry
};

typedef struct {
    object o;
    Console* con;
} IOStreamObject;
cpstatus iostream_class_entry(thread* t,const U8* name,const U8* desc,NativeClassOperations operations)
{
    switch (operations) {
        case NATIVE_GETSTATIC: case NATIVE_PUTSTATIC: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_GETFIELD: case NATIVE_PUTFIELD: {
            //We do NOT expose ANYTHING!
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_INVOKE: {
            for(U64 i=0;i<sizeof(g_iostream_methods)/sizeof(NativeMethod);i++) {
                if(!strcomp((char*)name,(char*)g_iostream_methods[i].name)
                && !strcomp((char*)desc,(char*)g_iostream_methods[i].desc)) {
                    return g_iostream_methods[i].method(t);
                }
            }
            except(t,"native method not found");
            return COLD_POINT_NOT_IMPLEMENTED;
        }
        case NATIVE_NEW: {
            IOStreamObject* obj = memalloc(sizeof(IOStreamObject));
            obj->o.parent_class = (class*)&g_iostream_class_linked_node.c;
            obj->o.var_count    = 0;
            t->stack[++t->rsp].data = (U64)obj;
            t->stack[t->rsp].type = STACK_TYPE_REFERENCE;
            return COLD_POINT_SUCCESS;
        }
    }
    return COLD_POINT_SUCCESS;
}
cpstatus iostream_class_register(InlineLinkedList* loaded_class_list)
{
    insert_existing_inline_node(loaded_class_list,&g_iostream_class_linked_node.node,-1);
    return COLD_POINT_SUCCESS;
}
