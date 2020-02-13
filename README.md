# fzOS-kernel
Kernel source code for fzOS.

# File Structure

drivers 里面大多是抄来的简版驱动

## notice
### 三定义用的头文件：c-99 标准的主要是uint类型；

+ <stdbool.h>
+ <stddef.h>
+ <stdint.h>

### 引用链
#### 里面的引用应该都是乱的
+ kernel display 用了 graphics.c ;
+ graphics.c 用了graphics.h;
+ graphics.h 调用了efi;

### 功能说明

+ graphics用efi的gop接口刷写显存实现位图输出；
+ kernel display 初始化一个字符模式的输入输出接口；

### 开发路线

#### 领袖说过：自己动手，丰衣足食。

+ Step1: 用图形模式实现一个字符显示输出；（helloworld）
+ Step2: 实现内存管理；
+ Step3: 实现进程/或者输入输出总线；
+ Step4: 这俩看着办；