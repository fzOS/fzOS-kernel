//设备树结构。
#ifndef DEVICE_TREE
#define DEVICE_TREE
#include <common/tree.h>
#include <drivers/blockdev.h>
#include <limit.h>
//一个设备结点16个字母的名字怎么也够了……
#define DT_NAME_LENGTH_MAX 16
extern inline_tree device_tree;

//定义设备树的结点类型。
typedef enum {
    DT_BRANCH,//非终结结点
    DT_CHAR_DEVICE,//字符设备
    DT_BLOCK_DEVICE,//块设备
    DT_FILESYSTEM,//挂载的文件系统
} DTNodeTypes;

typedef struct {
    inline_tree_node node;
    DTNodeTypes type;
    char name[DT_NAME_LENGTH_MAX];
}device_tree_node;

typedef enum {
    DT_CREATE_IF_NONEXIST,
    DT_RETURN_IF_NONEXIST
} DtResolveMethod;

typedef enum {
    DT_DESTROY_AFTER_REPLACE,
    DT_KEEP_AFTER_REPLACE
} DtDestroyMethod;
void init_device_tree();
void device_tree_add_from_parent(device_tree_node* n,device_tree_node* parent);
device_tree_node* device_tree_resolve_by_path(const char* full_path,DtResolveMethod method);
void device_tree_add_by_path(device_tree_node* n,char* c);
int device_tree_replace_node(device_tree_node* old,device_tree_node* new,DtDestroyMethod method);
device_tree_node* device_tree_resolve_from_parent(device_tree_node* n,char* node_name);
void print_device_tree();
#endif
