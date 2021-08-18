#include <coldpoint/automata/const_inst.h>
#include <common/bswap.h>
cpstatus opcode_nop(thread* t) {
    print_opcode("nop\n");
    (void)t;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_aconst_null(thread* t) {
    print_opcode("aconst_null\n");
    t->stack[t->rsp]=0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dconst_0(thread* t) {
    print_opcode("dconst_0\n");
    *(double*)(&t->stack[t->rsp])=0.0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_dconst_1(thread* t) {
    print_opcode("dconst_1\n");
    *(double*)(&t->stack[t->rsp])=1.0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_fconst_0(thread* t) {
    print_opcode("fconst_0\n");
    *(double*)(&t->stack[t->rsp])=0.0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_fconst_1(thread* t) {

    print_opcode("fconst_1\n");
    *(double*)(&t->stack[t->rsp])=1.0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_fconst_2(thread* t) {

    print_opcode("fconst_2\n");
    *(double*)(&t->stack[t->rsp])=2.0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_m1(thread* t) {
    print_opcode("iconst_m1\n");
    t->stack[t->rsp]=((int)-1);
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_0(thread* t) {
    print_opcode("iconst_0\n");
    t->stack[t->rsp]=0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_1(thread* t) {
    print_opcode("iconst_1\n");
    t->stack[t->rsp]=1;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_2(thread* t) {
    print_opcode("iconst_2\n");
    t->stack[t->rsp]=2;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_3(thread* t) {
    print_opcode("iconst_3\n");
    t->stack[t->rsp]=3;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_4(thread* t) {
    print_opcode("iconst_4\n");
    t->stack[t->rsp]=4;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_iconst_5(thread* t) {
    print_opcode("iconst_5\n");
    t->stack[t->rsp]=5;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lconst_0(thread* t) {
    print_opcode("lconst_0\n");
    t->stack[t->rsp]=0;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_lconst_1(thread* t) {
    print_opcode("lconst_1\n");
    t->stack[t->rsp]=1;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_bipush(thread* t) {
    I8 val = t->code->code[t->pc];
    print_opcode("bipush %d\n",val);
    t->stack[t->rsp]=(I64)val;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
cpstatus opcode_sipush(thread* t) {
    I16 val = bswap16(*(U16*)&t->code->code[t->pc]);
    print_opcode("sipush %d\n",val);
    t->stack[t->rsp]=(I64)val;
    t->rsp++;
    return COLD_POINT_SUCCESS;
}
