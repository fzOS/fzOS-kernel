#include <drivers/devicetree.h>
#include <common/kstring.h>
#include <memory/memory.h>
#include <common/printk.h>
inline_tree device_tree;
char* BASE_DEVICE_TREE_TEMPLATE = "/Devices/";
void init_device_tree(void)
{
    //采用了有头结点的格式，所以需要新建一个child.
    device_tree.entry.child = allocate_page(1);
    memset(device_tree.entry.child,0,sizeof(DeviceTreeNode));
    device_tree.entry.child->parent = &device_tree.entry;
    ((DeviceTreeNode*)device_tree.entry.child)->type = DT_BRANCH;
    memcpy(((DeviceTreeNode*)device_tree.entry.child)->name,"/",2);
}
void device_tree_add_by_path(DeviceTreeNode* n,char* c)
{
    //首先，解析。
    DeviceTreeNode* parent = device_tree_resolve_by_path(c,nullptr,DT_CREATE_IF_NONEXIST);
    //偷懒，直接插入。
    n->node.sibling = parent->node.child;
    n->node.parent = (inline_tree_node*)parent;
    parent->node.child = (inline_tree_node*)n;
}
void device_tree_add_from_parent(DeviceTreeNode* n,DeviceTreeNode* parent)
{
    //偷懒，直接插入。
    n->node.sibling = parent->node.child;
    n->node.parent = (inline_tree_node*)parent;
    parent->node.child = (inline_tree_node*)n;
}
DeviceTreeNode* device_tree_resolve_by_path(const char* full_path,const char** remaining,DtResolveMethod method)
{
    //Revision Using strmid().
    if(!strcomp(full_path,"/")) {
        if(method==DT_RETURN_LAST_PARENT) {
            if(remaining!=nullptr) {
                *remaining = full_path+1;
            }
        }
        return (DeviceTreeNode*)device_tree.entry.child;
    }
    char buf[FILENAME_MAX];
    const char* p=full_path+1;
    DeviceTreeNode *node = (DeviceTreeNode*)(device_tree.entry.child);
    DeviceTreeNode *parent = node;
    int len=1;
    while(len) {
        len = strmid(buf,FILENAME_MAX,(char*)p,PATH_SEPARATOR);
        if(!*buf) {
            break;
        }
        node = device_tree_resolve_from_parent(node,buf);
        if(node==nullptr) {
            switch(method) {
                case DT_CREATE_IF_NONEXIST: {
                    node = allocate_page(1);
                    memset(node,0,sizeof(DeviceTreeNode));
                    node->type = DT_BRANCH;
                    strcopy(node->name,buf,DT_NAME_LENGTH_MAX);
                    node->node.sibling = parent->node.child;
                    node->node.parent = (inline_tree_node*)parent;
                    parent->node.child = (inline_tree_node*)node;
                    return node;
                }
                case DT_RETURN_IF_NONEXIST: {
                    return nullptr;
                }
                case DT_RETURN_LAST_PARENT: {
                    if(remaining!=nullptr) {
                        *remaining = p;
                        return parent;
                    }
                }
            }
        }
        parent = node;
        p += len;
        if(*p==PATH_SEPARATOR) {
            p++;
        }
        else {
            break;
        }
    }
    if(method==DT_RETURN_LAST_PARENT) {
         if(remaining!=nullptr) {
            *remaining = p;
         }
    }
    return node;
}

DeviceTreeNode* device_tree_resolve_from_parent(DeviceTreeNode* n,char* node_name)
{
    DeviceTreeNode* node =(DeviceTreeNode*)(n->node.child);
    while(node!=nullptr) {
        if(strcomp(node_name,node->name)) {
            node = (DeviceTreeNode*)node->node.sibling;
        }
        else {
            break;
        }
    }
    return node;
}
void print_device_tree(void)
{
    iterator(inline_tree) iter;
    init_iterator(inline_tree,&iter,&device_tree);
    while(iter.next(&iter)) {
        for(int i=0;i<iter.count-1;i++) {
            printk("--");
        }
        printk("%s\n",((DeviceTreeNode*)iter.current)->name);
    }
}
int device_tree_replace_node(DeviceTreeNode* old,DeviceTreeNode* new,DtDestroyMethod method)
{
    new->node.parent = old->node.parent;
    new->node.child  = old->node.child;
    new->node.sibling = old->node.sibling;
    old->node.parent->child = &(new->node);
    if(method==DT_DESTROY_AFTER_REPLACE) {
        free_page(old,1);
    }
    return FzOS_SUCCESS;
}
