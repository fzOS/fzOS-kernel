//内核中的树结构定义，
#ifndef TREE_H
#define TREE_H
#include <types.h>
#include <common/iterator.h>
//使用“孩子-兄弟”结点结构储存信息。
typedef struct tree_node {
    struct tree_node* child;
    struct tree_node* parent;
    struct tree_node* sibling;
} inline_tree_node;

typedef struct {
    inline_tree_node entry; //入口
} inline_tree;
typedef iterator_raw(inline_tree) iterator_inline_tree;
void init_iterator_inline_tree(iterator(inline_tree)* iterator,inline_tree* source);
void insert_tree_node_as_child(inline_tree_node* parent,inline_tree_node* data);
#endif
