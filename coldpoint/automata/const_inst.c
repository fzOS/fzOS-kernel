#include <coldpoint/automata/const_inst.h>
#include <common/bswap.h>
_Static_assert(sizeof(double)==sizeof(U64),"sizeof(Double)!=sizeof(U64)");
_Static_assert(sizeof(float)==sizeof(U32),"sizeof(float)!=sizeof(U32)");
cpstatus opcode_nop(thread* t) {
    print_opcode("nop\n");
    (void)t;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_aconst_null(thread* t) {
    print_opcode("aconst_null\n");
    t->stack[t->rsp].data=0;
    t->stack[t->rsp].type=STACK_TYPE_REFERENCE;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dconst_0(thread* t) {
    print_opcode("dconst_0\n");
    double* sp = (double*)(&(t->stack[t->rsp].data));
    *sp=0.0;
    t->stack[t->rsp].type=STACK_TYPE_DOUBLE;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dconst_1(thread* t) {
    print_opcode("dconst_1\n");
    double* sp = (double*)(&(t->stack[t->rsp].data));
    *sp=1.0;
    t->stack[t->rsp].type=STACK_TYPE_DOUBLE;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_fconst_0(thread* t) {
    print_opcode("fconst_0\n");
    double* sp = (double*)(&(t->stack[t->rsp].data));
    *sp=0.0;
    t->stack[t->rsp].type=STACK_TYPE_FLOAT;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_fconst_1(thread* t) {

    print_opcode("fconst_1\n");
    double* sp = (double*)(&(t->stack[t->rsp].data));
    *sp=1.0;
    t->stack[t->rsp].type=STACK_TYPE_FLOAT;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_fconst_2(thread* t) {

    print_opcode("fconst_2\n");
    double* sp = (double*)(&(t->stack[t->rsp].data));
    *sp=2.0;
    t->stack[t->rsp].type=STACK_TYPE_FLOAT;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_m1(thread* t) {
    print_opcode("iconst_m1\n");
    t->stack[t->rsp].data=-1;
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_0(thread* t) {
    print_opcode("iconst_0\n");
    t->stack[t->rsp].data=0;
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_1(thread* t) {
    print_opcode("iconst_1\n");
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=1;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_2(thread* t) {
    print_opcode("iconst_2\n");
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=2;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_3(thread* t) {
    print_opcode("iconst_3\n");
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=3;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_4(thread* t) {
    print_opcode("iconst_4\n");
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=4;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_5(thread* t) {
    print_opcode("iconst_5\n");
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=5;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lconst_0(thread* t) {
    print_opcode("lconst_0\n");
    t->stack[t->rsp].type=STACK_TYPE_LONG;
    t->stack[t->rsp].data=0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lconst_1(thread* t) {
    print_opcode("lconst_1\n");
    t->stack[t->rsp].type=STACK_TYPE_LONG;
    t->stack[t->rsp].data=1;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_bipush(thread* t) {
    I8 val = t->code->code[t->pc];
    print_opcode("bipush %d\n",val);
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=(I32)val;
    t->rsp++;
    t->pc++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_sipush(thread* t) {
    I16 val = bswap16(*(U16*)&t->code->code[t->pc]);
    print_opcode("sipush %d\n",val);
    t->stack[t->rsp].type=STACK_TYPE_INT;
    t->stack[t->rsp].data=(I32)val;
    t->rsp++;
    t->pc+=sizeof(U16);
    return COLD_POINT_SUCCESS;
}
