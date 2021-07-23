#include <common/tree.h>

int iterator_tree_next(iterator(tree)* this)
{
    //我们只支持先根遍历（这也是用的最多的）
    //换句话说，自己-孩子-兄弟，即深度有限遍历（DFS）
    //别的遍历方法？见鬼去吧！#（滑稽）
    if(this->current->child) {
        this->current = this->current->child;
        this->count++;
        return 1;
    }
    if(this->current->sibling) {
        this->current = this->current->sibling;
        return 1;
    }
    if(this->current->parent->sibling) {
        this->current = this->current->parent->sibling;
        this->count--;
        return 1;
    }
    return 0;
}
int iterator_tree_prev(iterator(tree)* this)
{
    if(this->current==(&this->list->entry)) {
        return 0;
    }
    if(this->current->parent->child==this->current) {
        //一支独苗，前一个就是它父亲。
        this->current = this->current->parent;
        this->count--;
        return 1;
    }
    tree_node* child = this->current->parent->child;
    while(child->sibling!=this->current) {
        this->current = child->sibling;
        child = child->sibling;
    }
    this->current = child;
    return 1;
}
void iterator_tree_remove(iterator(tree)* this)
{
    //暂时不实现删除。
    return;
}
void init_iterator_tree_node(iterator(tree)* iterator,tree* source)
{
    iterator->list = source;
    iterator->count = 0;
    iterator->current = &(source->entry);
    iterator->next = (void*)iterator_tree_next;
    iterator->prev = (void*)iterator_tree_prev;
    iterator->remove = (void*)iterator_tree_remove;
}
void insert_tree_node_as_child(tree_node* parent,tree_node* data)
{
    if(parent->child) {
        tree_node* child = parent->child;
        while(child->sibling) {
            child = child->sibling;
        }
        child->sibling = data;
        data->parent = child->parent;
        return;
    }
    parent->child = data;
    data->parent = parent;
}
