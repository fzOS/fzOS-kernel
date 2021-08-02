#include <drivers/devicetree.h>
#include <common/kstring.h>
#include <memory/memory.h>
#include <common/printk.h>
inline_tree device_tree;
void init_device_tree()
{
    //采用了有头结点的格式，所以需要新建一个child.
    device_tree.entry.child = allocate_page(1);
    memset(device_tree.entry.child,0,sizeof(device_tree_node));
    device_tree.entry.child->parent = &device_tree.entry;
    ((device_tree_node*)device_tree.entry.child)->type = DT_BRANCH;
    memcpy(((device_tree_node*)device_tree.entry.child)->name,"/",2);
}
void device_tree_add_by_path(device_tree_node* n,char* c)
{
    //首先，解析。
    device_tree_node* parent = device_tree_resolve_by_path(c,DT_CREATE_IF_NONEXIST);
    //偷懒，直接插入。
    n->node.sibling = parent->node.child;
    n->node.parent = (inline_tree_node*)parent;
    parent->node.child = (inline_tree_node*)n;
}
void device_tree_add_from_parent(device_tree_node* n,device_tree_node* parent)
{
    //偷懒，直接插入。
    n->node.sibling = parent->node.child;
    n->node.parent = (inline_tree_node*)parent;
    parent->node.child = (inline_tree_node*)n;
}
device_tree_node* device_tree_resolve_by_path(const char* full_path,DtResolveMethod method)
{
    if(!strcomp(full_path,"/")) {
        return (device_tree_node*)device_tree.entry.child;
    }
    char buf[FILENAME_MAX];
    const char *front,*rear;
    device_tree_node *node = (device_tree_node*)(device_tree.entry.child);
    device_tree_node *parent = node;
    front = full_path;
    rear = full_path+1;
    while(*rear!='\0') {
        if(*front!=PATH_SEPARATOR) {
            front++;
        }
        if(*rear==PATH_SEPARATOR) {
            memcpy(buf,(char*)front+1,rear-front-1);
            buf[rear-front-1]='\0';;
            node = device_tree_resolve_from_parent(node,buf);
            if(node==nullptr) {
                if(method==DT_CREATE_IF_NONEXIST) {
                    node = allocate_page(1);
                    memset(node,0,sizeof(device_tree_node));
                    node->type = DT_BRANCH;
                    strcopy(node->name,buf,DT_NAME_LENGTH_MAX);
                    node->node.sibling = parent->node.child;
                    node->node.parent = (inline_tree_node*)parent;
                    parent->node.child = (inline_tree_node*)node;
                }
                else {
                    return nullptr;
                }
            }
            parent = node;
            front = rear;
        }
        rear++;
    }
    if(*(rear-1)!=PATH_SEPARATOR) {
        //如果不以/结尾，那么说明还有残存的数据。
        memcpy(buf,(char*)front+1,rear-front-1);
        buf[rear-front-1]='\0';
        node = device_tree_resolve_from_parent(node,buf);
        if(node==nullptr) {
            if(method==DT_CREATE_IF_NONEXIST) {
                device_tree_node* new_parent = allocate_page(1);
                new_parent->type = DT_BRANCH;
                memset(new_parent,0,sizeof(device_tree_node));
                strcopy(new_parent->name,buf,DT_NAME_LENGTH_MAX);
                new_parent->node.sibling = parent->node.child;
                new_parent->node.parent = (inline_tree_node*)parent;
                parent->node.child = &(new_parent->node);
                return new_parent;
            }
            else {
                return nullptr;
            }
        }
    }
    return node;
}

device_tree_node* device_tree_resolve_from_parent(device_tree_node* n,char* node_name)
{
    device_tree_node* node =(device_tree_node*)(n->node.child);
    while(node!=nullptr) {
        if(strcomp(node_name,node->name)) {
            node = (device_tree_node*)node->node.sibling;
        }
        else {
            break;
        }
    }
    return node;
}
void print_device_tree()
{
    iterator(inline_tree) iter;
    init_iterator(inline_tree,&iter,&device_tree);
    while(iter.next(&iter)) {
        for(int i=0;i<iter.count-1;i++) {
            printk("--");
        }
        printk("%s\n",((device_tree_node*)iter.current)->name);
    }
}
int device_tree_replace_node(device_tree_node* old,device_tree_node* new,DtDestroyMethod method)
{
    new->node.parent = old->node.parent;
    new->node.child  = old->node.child;
    new->node.sibling = old->node.sibling;
    old->node.parent->child = &(new->node);
    if(method==DT_DESTROY_AFTER_REPLACE) {
        free_page(old,1);
    }
    return FzOS_SUCEESS;
}
