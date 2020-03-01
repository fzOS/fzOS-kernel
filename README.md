# fzOS-kernel
Kernel source code for fzOS.

## 开发路线
### 这块现在理清了
#### 领袖说过：自己动手，丰衣足食。
#### burg引导器同步开发中

+ [x]Step1: 用图形模式实现一个字符显示输出；（helloworld）这个完成了，kernel log 支持字符串输出;
+ [x]Step2: 基本实现了U64的16进制和10进制输出;
+ [ ]Step3: 实现CPU段表的设置, 接管内存分配;
+ [ ]Step4: 实现基础的中断程序和TSS任务表设置;
+ [ ]后面看着办先

# File Structure

### 定义用的头文件

+ <types.h> 定义了基础整型单位;
+ <uefivars.h> 定义了与UEFI相关的代码需要包含的头文件;

## 基本的<drivers>函数库
### graphics.c;
+ 初始化屏幕;
+ 实现清屏;
+ 刷写像素;
+ 计算字符相对位置;
+ 屏幕内容整体上移;

### kerneldisplay.c;
+ 实现ASCII字符输出;
+ 实现变量的10进制和16进制显示;
+ 实现换行和满屏后向上刷新;

### asciifont.c;
+ 基础ASCII字符库;