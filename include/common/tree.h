//内核中的树结构定义，
#ifndef TREE_H
#define TREE_H
#include <types.h>
#include <common/iterator.h>
//使用“孩子-兄弟”结点结构储存信息。
typedef struct TreeNode {
    struct TreeNode* child;
    struct TreeNode* parent;
    struct TreeNode* sibling;
} InlineTreeNode;

typedef struct {
    InlineTreeNode entry; //入口
} InlineTree;
typedef IteratorRaw(InlineTree) IteratorInlineTree;
void init_iterator_InlineTree(Iterator(InlineTree)* iterator,InlineTree* source);
void insert_tree_node_as_child(InlineTreeNode* parent,InlineTreeNode* data);
#endif
