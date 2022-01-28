<div align="center"><img src="logo.png" style="zoom:75%;" /></div>

# fzOS-kernel

#### Kernel source code for fzOS.
#### fzOS内核源码，
#### 这是一个充满了神奇实现的 AMD64/X86-64 内核
#### 目前fzOS只支持也只打算支持UEFI启动
#### 也不打算支持32位（毕竟谁2202年了谁还用32位）

## External Libraries

LAI(https://github.com/managarm/lai/) for ACPI Parsing

## Note for compiler 关于编译器版本可能对当前版本kernel导致的编译失败
+ lower/equal than gcc-10 is not suitable due to the asm goto support 
+ 请使用高于 gcc-10 的 GNU-GCC编译器进行编译 （由于random部分使用了asm汇编的goto，其中的部分方法在gcc<=10不被支持）
    + link (https://gcc.gnu.org/onlinedocs/gcc-10.2.0/gcc/Extended-Asm.html#Goto-Labels)
    + 链接 (https://gcc.gnu.org/onlinedocs/gcc-10.2.0/gcc/Extended-Asm.html#Goto-Labels)
    + Before GCC-11, an asm goto statement cannot have outputs. 
    + 在 gcc-11 之前，asm goto 声明不支持 output 参数。
    + further link (https://github.com/gcc-mirror/gcc/blob/releases/gcc-10/gcc/c/c-parser.c)
    + 链接 (https://github.com/gcc-mirror/gcc/blob/releases/gcc-10/gcc/c/c-parser.c)
    + In GCC-10, For asm goto, we don't allow output operands, but reserve the slot for a future extension that does allow them.
    + 在 gcc-10 中，对于 asm goto 指令，暂时不支持输出操作，但是这个指令位置被保留，作为对未来可能更新的预留

## Install/Compile Guidence 安装/编译说明
#### 编译环境
+ 操作系统
    + 请使用 linux 作为编译环境 (其中 debian 和 arch linux 分别是我们的开发环境)
    + macOS下有许多奇怪的问题，反正我是没成功编译，而且结果需要是elf而不是mach-O
    + 没人用 windows, 所以就不讨论这个系统了
    + docker 使用 build 的 gcc11 会有一些奇怪的问题
+ 软件和编译器
    + 需要：GCC, build-essential, gnu-efi
    + GCC需要高于版本11, 目前开发使用的是 GCC-11.2.0
    + clang 由于asm语法上的问题，可能需要一些小小的改动
    + 内联 ASM 用的是 AT&T 语法
+ 编译
    + 在文件夹里 make 就行了
    + 结果是 build/kernel 这个可执行文件
+ 运行/测试
    + 去隔壁 fzBurg 找一下 efi 引导文件
    + 直接用vso文件，
    + 需要另一个linux虚拟机设置磁盘系统
    + 然后去虚拟机

## 命名规范
#### 请尤其注意下列保留区段.jpg
+ 变量
    + 常规变量使用 小写下划线命名 ( 例如: bss_info )
    + 全局变量使用 g_小写下划线命名（ 例如: g_gui_service_window_list ）
+ 宏定义
    + 全大写下划线命名 （ 例如: GUI_DEFAULT_WINDOW_SIZE ）
    + 下划线开头结尾的全大写（ 例如: _GUI_DEFAULT_WINDOW_SIZE_ ）
+ 结构体
    + 驼峰命名 （ 例如: TestVariableType ）
+ 函数
    + 小写下划线

## 开发路线
### 这块现在理清了
#### 领袖说过：自己动手，丰衣足食。
#### burg引导器同步开发中

+ [x]Step1: 用图形模式实现一个字符显示输出；（helloworld）这个完成了，kernel log 支持字符串输出;
+ [x]Step2: 基本实现了U64的16进制和10进制输出;
+ [x]Step3: 实现CPU段表的设置, 接管内存分配;
+ [x]Step4: 实现基础的中断程序和TSS任务表设置;
+ [x]Step5: 实现基础的键盘驱动程序;
+ [x]Step6: 实现完整的ACPI支持，完成“高级电源管理”;
+ [x]Step7: 实现文件系统;
+ [x]Step8: 实现设备树;
+ [x]Step9: 实现JVM的类加载器;
+ [&ensp;]Step10: 实现JVM的基础指令;
+ [&ensp;]Step11: 实现JVM的堆机制;
+ [&ensp;]Step12: 实现System类，与Kernel ABI交互;
+ [&ensp;]Step13: 与JVM并行线，GUI开发
+ [x]Step14: 与JVM并行线，实现HDA声卡驱动;
+ [x]Step15: 实现WAV格式的音频播放;

# File Structure

### 定义用的头文件

+ <types.h> 定义了基础整型单位;
+ <uefivars.h> 定义了与UEFI相关的代码需要包含的头文件;
+ <limit.h> 定义了当前系统的一些限制;

---
## common

### cpuid.c;
+ 实现cpu信息的获取;

### file.c;
+ 基本文件结构体的实现;

### halt.c;
+ 基本CPU状态管理（停机+死机）;

### io.c;
+ I/O操作的简单包装;

### kstring.c;
+ 简单的内存/字符串操作;

### linkedlist.c;
+ 数据结构（链表）的实现:双向链表;

### popcount.c;
+ 实现POPCNT指令;

### power_control.c;
+ 高级电源管理（关机/重启）的实现;

### printk.c;
+ 实现变量的10进制和16进制显示;

### queue.c;
+ 实现队列数据结构;

### registers.c;
+ msr寄存器的操作;

### semaphore.c & spinlock.c;
+ 两种进程同步的结构体(信号量/自旋锁);

---
## drivers

### devicetree.c;
+ 设备树的结构;

### graphics.c;
+ 初始化屏幕;
+ 实现清屏;
+ 刷写像素;
+ 计算字符相对位置;
+ 屏幕内容整体上移;

### fbcon.c;
+ 实现ASCII字符输出;
+ 实现换行和满屏后向上刷新;

### gpt.c;
+ GUID分区图的识别;

### keyboard.c;
+ 基础键盘的处理;

### pci_bus.c;
+ PCI总线驱动;

### rtc.c;
+ 实时时钟驱动;

### sata_ahci.c;
+ AHCI模式的Serial ATA设备驱动;

---
## filesystem

### filesystem.c
+ 通用文件系统操作；

### fhhfs.c
+ fhhfs文件系统驱动；

---
## interrupt

### interrupt.c;
+ 异常以及irq的声明;

### irq.c;
+ 外部中断芯片的初始化;

---
## memory

### gdt.c;
+ 设置cpu段表;

### memory.c;
+ 内存的分配与回收;

### memorysetup.c;
+ 内存空闲空间的接管;
