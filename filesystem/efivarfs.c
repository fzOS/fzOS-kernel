#include <filesystem/efivarfs.h>
#include <memory/memory.h>
int mount_efivars(EFI_RUNTIME_SERVICES *rt)
{
    EFIvarfsTreeNode* node = (EFIvarfsTreeNode*)device_tree_resolve_by_path("/EFIVariables",nullptr,DT_CREATE_IF_NONEXIST);
    memset(&node->fs,0,sizeof(node->fs));
    node->node.type = DT_FILESYSTEM;
    node->fs.rt = rt;
    return FzOS_SUCCESS;
}
