//内核中的树结构定义，
#ifndef TREE_H
#define TREE_H
#include <types.h>
#include <common/iterator.h>
//使用“孩子-兄弟”结点结构储存信息。
typedef struct tree_node {
    struct tree_node* child;
    struct tree_node* parent;
    void* data;
    struct tree_node* sibling;
} tree_node;

typedef struct {
    tree_node entry; //入口
} tree;
typedef iterator_raw(tree) iterator_tree;
void insert_tree_node_as_child(tree_node* parent,tree_node* data);
#endif
