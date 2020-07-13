#ifndef MEMORY_TABLES
#define MEMORY_TABLES
#include <types.h>
typedef union {
    struct {
        int P   :1;//0,P
        int RW  :1;//1,R/W
        int US  :1;//2,U/S
        int PWT :1;//3,PWT
        int PCD :1;//4,PCD
        int A   :1;//5,A
        int     :1;//6,Ignored
        int Zero:2;//7~8,0
        int AVL1:3;//9~11,AVL
        U64 PDBA:40;//12~51,Page-Directory Base Address
        int AVL2:11;//52~62,Available
        int NX  :1; //63,NX bit
    } __attribute((packed)) split;
    U64 raw;
} PML4E; //四级页表
typedef union {
    struct {
        int P   :1;//0,P
        int RW  :1;//1,R/W
        int US  :1;//2,U/S
        int PWT :1;//3,PWT
        int PCD :1;//4,PCD
        int A   :1;//5,A
        int     :1;//6,Ignored
        int Zero:1;//7,0
        int     :1;//8,Ignored
        int AVL1:3;//9~11,AVL
        U64 PDBA:40;//12~51,Page-Directory Base Address
        int AVL2:11;//52~62,Available
        int NX  :1; //63,NX bit
    } __attribute((packed)) split;
    U64 raw;
} PDPE; //三级页表
typedef union {
    struct {
        int P   :1;//0,P
        int RW  :1;//1,R/W
        int US  :1;//2,U/S
        int PWT :1;//3,PWT
        int PCD :1;//4,PCD
        int A   :1;//5,A
        int     :1;//6,Ignored
        int Zero:1;//7,0
        int     :1;//8,Ignored
        int AVL1:3;//9~11,AVL
        U64 PDBA:40;//12~51,Page-Directory Base Address
        int AVL2:11;//52~62,Available
        int NX  :1; //63,NX bit
    } __attribute((packed)) split;
    U64 raw;
} PDE; //二级页表
typedef union {
    struct {
        int P   :1;//0,P
        int RW  :1;//1,R/W
        int US  :1;//2,U/S
        int PWT :1;//3,PWT
        int PCD :1;//4,PCD
        int A   :1;//5,A
        int D   :1;//6,D
        int PAT :1;//7,PAT
        int G   :1;//8,G
        int AVL1:3;//9~11,AVL
        U64 PDBA:40;//12~51,Page-Directory Base Address
        int AVL2:11;//52~62,Available
        int NX  :1; //63,NX bit
    } __attribute((packed)) split;
    U64 raw;
} PTE; //一级页表

typedef union {
    struct {
        U64 physical_page_offset:12;//0~11
        U64 page_table_offset:9    ;//12~20
        U64 page_directory_offset:9;//21~29
        U64 page_directory_pointer_offset:9;//30~38
        U64 page_map_level_4_offset:9;//39~47,PML4
        U64 sign_extended:16;//48~63
    } __attribute((packed)) split;
    U64 raw;
} memory_address;
#endif