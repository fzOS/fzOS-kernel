#include <common/file.h>
#include <drivers/devicetree.h>
#include <common/printk.h>
int generic_open(char* filename,file* file)
{
    //思路：从设备树开始parse,当not found时，调用文件系统内部的open,
    const char* remaining;
    DeviceTreeNode* node = device_tree_resolve_by_path(filename,&remaining,DT_RETURN_LAST_PARENT);
    switch(node->type) {
        //FIXME:实现未实现的方法。
        case DT_BLOCK_DEVICE: {
            printk(" Stub!");
            return FzOS_NOT_IMPLEMENTED;
        }
        case DT_BRANCH: {
            //Branch不能被打开！
            printk(" Stub!");
            return FzOS_NOT_IMPLEMENTED;
        }
        case DT_CHAR_DEVICE: {
            printk(" Stub!");
            return FzOS_NOT_IMPLEMENTED;
        }
        case DT_FILESYSTEM: {
            //链式调用后面的open。
            FileSystemTreeNode* fs_node = (FileSystemTreeNode*) node;
            return fs_node->fs.open(&(fs_node->fs),(char*)(remaining-1),file);
        }
    }
    return FzOS_SUCCESS;
}
