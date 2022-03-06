#ifndef OBJ_INST
#define OBJ_INST
#include <coldpoint/threading/thread.h>
typedef enum {
    T_REFERENCE=0, //Extension to existing JVM.
    T_BOOLEAN = 4,
    T_CHAR,
    T_FLOAT,
    T_DOUBLE,
    T_BYTE,
    T_SHORT,
    T_INT,
    T_LONG
} ArrayType;
typedef struct {
    U64 length;
    const U8* type;
    U64 value[0];
} Array;
cpstatus opcode_ldc(thread* t);
cpstatus opcode_ldc2_w(thread* t);
cpstatus opcode_new(thread* t);
cpstatus opcode_newarray(thread* t);
cpstatus opcode_anewarray(thread* t);
cpstatus opcode_getstatic(thread* t);
cpstatus opcode_putstatic(thread* t);
cpstatus opcode_getfield(thread* t);
cpstatus opcode_putfield(thread* t);
cpstatus opcode_instanceof(thread* t);
cpstatus opcode_checkcast(thread* t);
cpstatus opcode_iaload(thread* t);
cpstatus opcode_laload(thread* t);
cpstatus opcode_faload(thread* t);
cpstatus opcode_daload(thread* t);
cpstatus opcode_aaload(thread* t);
cpstatus opcode_baload(thread* t);
cpstatus opcode_caload(thread* t);
cpstatus opcode_saload(thread* t);
#endif
