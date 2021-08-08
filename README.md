<div align="center"><img src="logo.png" style="zoom:75%;" />

# fzOS-kernel

Kernel source code for fzOS.

## 开发路线
### 这块现在理清了
#### 领袖说过：自己动手，丰衣足食。
#### burg引导器同步开发中

+ [x]Step1: 用图形模式实现一个字符显示输出；（helloworld）这个完成了，kernel log 支持字符串输出;
+ [x]Step2: 基本实现了U64的16进制和10进制输出;
+ [x]Step3: 实现CPU段表的设置, 接管内存分配;
+ [x]Step4: 实现基础的中断程序和TSS任务表设置;
+ [&ensp;]Step5: 实现基础的键盘驱动程序;
+ [x]Step6: 实现完整的ACPI支持，完成“高级电源管理”;
+ [x]Step7: 实现文件系统;
+ [x]Step8: 实现设备树;
+ [&ensp;]Step9: 实现JVM的类加载器;
+ [&ensp;]Step10: 实现JVM的基础指令;
+ [&ensp;]后面看着办先

# File Structure

### 定义用的头文件

+ <types.h> 定义了基础整型单位;
+ <uefivars.h> 定义了与UEFI相关的代码需要包含的头文件;
+ <limit.h> 定义了当前系统的一些限制;

## common

### cpuid.c;
+ 实现cpu信息的获取;

### halt.c;
+ 基本CPU状态管理（停机+死机）;

### io.c;
+ I/O操作的简单包装;

### kstring.c;
+ 简单的内存/字符串操作;

### linkedlist.c;
+ 数据结构（链表）的实现:双向链表;

### power_control.c;
+ 高级电源管理（关机/重启）的实现;

### printk.c;
+ 实现变量的10进制和16进制显示;

### registers.c;
+ msr寄存器的操作;

## drivers

### graphics.c;
+ 初始化屏幕;
+ 实现清屏;
+ 刷写像素;
+ 计算字符相对位置;
+ 屏幕内容整体上移;

### fbcon.c;
+ 实现ASCII字符输出;
+ 实现换行和满屏后向上刷新;

### keyboard.c;
+ 基础键盘的处理;

## interrupt

### interrupt.c;
+ 异常以及irq的声明;

### irq.c;
+ 外部中断芯片的初始化;

## memory

### gdt.s & gdt.c;
+ 设置cpu段表;

### memory.c;
+ 内存的分配与回收;

### memorysetup.c;
+ 内存空闲空间的接管;
