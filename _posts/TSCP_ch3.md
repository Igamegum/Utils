---
title: "程序员的自我修养(第三章)"
tags: [笔记,程序员的自我修养]
---
目标文件从结构上讲是已经编译后的可执行文件格式，只是还没有经过链接的过程，其中可能有些符号或者地址还没有被调整

<!--more-->

## 目标文件的格式
PC平台的可执行文件格式．主要是Windows下的<b>PE</b>(Portable Executable)，Linux下的<b>ELF</b>(Executable Linkable Format),它们都是<b>COFF</b>(Common File Format)格式的变种．静态链接库和动态链接库等都按照可执行文件的格式存储.ELF文件格式归类如下表

|ELF文件类型|说明|实例|
|----------|----|----|
|Relocatable File|这类文件包含了代码和数据段，可以被用来链接成可执行文件或者共享目标文件，静态库也归为这一类|Linux的.o<br/>Windows的.obj|
|Executable File|这类文件包含了可以直接执行的程序，它的代表就是ELF可执行文件，它们一般都没有扩展名|Linux的/bin/bash<br/>Windows的.exe|
|Shared Object File|这类文件包含代码和数据，可以在一下两种情况下使用．一种是链接器可以使用这种文件跟其他的可重定位文件和共享目标文件链接，产生新的目标文件．第二种时动态链接器可以将几个这种共享目标文件和可执行文件结合，作为进程映像的一部分来运行|Linux的.so<br/>Windows的.dll|
|Core Dump File|当进程意外终止时，系统可以将该进程的地址空间的内容及终止时的一些其他信息转储到核心转储文件|Linux的core dump|

## 目标文件的结构
目标文件中的信息按照信息不同的属性，以<b>节</b>(Section)或者<b>段</b>(Segment)的形式存储，两者唯一的区别是在ELF的链接视图和装载视图的时候．ELF文件结构如下表

|名称|说明|
|----|---|
|File Header|ELF文件的开头时一个文件头，它描述了整个文件的文件属性．文件头还包含一个<b>段表</b>，段表是描述一个文件中各个段的数组，包括各个段在文件中的偏移位置以及段的属性|
|.text section|程序源代码编译后的机器指令经常放在此段|
|.data section|已初始化的全局变量和局部静态变量放在此段|
|.bss section|未初始化的全局变量和局部静态变量放在此段|
<b>BSS(Block Start by Symbol)</b>段只是为未初始化的全局变量和静态局部变量预留位置，并没有内容．简单地说，未初始化的全局变量和静态局部变量默认值都是０，所以没有必要为它们在段中分配空间并存放数据０


## 实例挖掘
在这里，我们以一个实例代码编译出的目标文件做分析，目标文件会因为编译器的版本以及机器平台不同而有所变化．以下例子以我的环境为例
系统:Ubuntu16.04 LTS
编译器:gcc 7.2.0
CPU:Intel(R) Core(TM) i5-6402P
代码
```
int printf (const char * format,...);

int global_init_var = 84;
int global_uninit_var;

void func1(int i)
{
	printf("%d\n",i);
}

int main(void)
{
	static int static_var = 85;
	static int static_var2;
	
	int a = 1;
	int b;
	
	func1( static_var + static_var2 + a + b);
	
	return a;
}
```

首先我们将源代码文件编译成目标文件
>gcc -c SimpleSection.c

得到了大小为1888byte的目标文件SimpleSection.o之后，我们运行如下命令
>objdump -h SimpleSection.o

得到如下内容

```
SimpleSection.o:     file format elf64-x86-64

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .text         00000055  0000000000000000  0000000000000000  00000040  2**0
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE
  1 .data         00000008  0000000000000000  0000000000000000  00000098  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  2 .bss          00000004  0000000000000000  0000000000000000  000000a0  2**2
                  ALLOC
  3 .rodata       00000004  0000000000000000  0000000000000000  000000a0  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .comment      0000002a  0000000000000000  0000000000000000  000000a4  2**0
                  CONTENTS, READONLY
  5 .note.GNU-stack 00000000  0000000000000000  0000000000000000  000000ce  2**0
                  CONTENTS, READONLY
  6 .eh_frame     00000058  0000000000000000  0000000000000000  000000d0  2**3
                  CONTENTS, ALLOC, LOAD, RELOC, READONLY, DATA

```

可以发现，除了上面提到的几个常见的段以外，还有<b>只读数据段(.rodata)</b>,<b>注释信息段(.comment)</b>,<b>堆栈提示段(.note.GNU-stack)</b>,<b>异常处理函数帧段(.eh_frame)</b>.
每个段的第二行都带有一些标签，这写标签代表这些段的属性，其中"CONTENTS"代表该段在ELF文件中存在．我们可以看到bss段并没有COMMENT标签，这跟我们上面说的，bss段没有内容的说法一致．那么，SimpleSection.o 在ELF中的结构大体如下图所示
![ELF](http://7xvs12.com1.z0.glb.clouddn.com/ELF.png)


* #### 代码段
我们用如下命令将所有段的内容以１６进制的方式打印出来
>objdump -s SimpleSection.o 

```

SimpleSection.o:     file format elf64-x86-64

Contents of section .text:
 0000 554889e5 4883ec10 897dfc8b 45fc89c6  UH..H....}..E...
 0010 bf000000 00b80000 0000e800 00000090  ................
 0020 c9c35548 89e54883 ec10c745 f8010000  ..UH..H....E....
 0030 008b1500 0000008b 05000000 0001c28b  ................
 0040 45f801c2 8b45fc01 d089c7e8 00000000  E....E..........
 0050 8b45f8c9 c3                          .E...           
Contents of section .data:
 0000 54000000 55000000                    T...U...        
Contents of section .rodata:
 0000 25640a00                             %d..            
Contents of section .comment:
 0000 00474343 3a202855 62756e74 7520372e  .GCC: (Ubuntu 7.
 0010 322e302d 31756275 6e747531 7e31362e  2.0-1ubuntu1~16.
 0020 30342920 372e322e 3000               04) 7.2.0.      
Contents of section .eh_frame:
 0000 14000000 00000000 017a5200 01781001  .........zR..x..
 0010 1b0c0708 90010000 1c000000 1c000000  ................
 0020 00000000 22000000 00410e10 8602430d  ...."....A....C.
 0030 065d0c07 08000000 1c000000 3c000000  .]..........<...
 0040 00000000 33000000 00410e10 8602430d  ....3....A....C.
 0050 066e0c07 08000000                    .n......        

```
其中最左边一列代表偏移量，最右边一列是.text的ASCII码形式,中间每一列为十六进制内容(每两个字符代表一个字节),可以看到.text段的长度正是0x55字节(ASCII码形式也刚好为55个字符)
再将所有包含的指令段反汇编
>objdump -d SimpleSection.o 

```
SimpleSection.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <func1>:
   0:	55                   	push   %rbp
   1:	48 89 e5             	mov    %rsp,%rbp
   4:	48 83 ec 10          	sub    $0x10,%rsp
   8:	89 7d fc             	mov    %edi,-0x4(%rbp)
   b:	8b 45 fc             	mov    -0x4(%rbp),%eax
   e:	89 c6                	mov    %eax,%esi
  10:	bf 00 00 00 00       	mov    $0x0,%edi
  15:	b8 00 00 00 00       	mov    $0x0,%eax
  1a:	e8 00 00 00 00       	callq  1f <func1+0x1f>
  1f:	90                   	nop
  20:	c9                   	leaveq 
  21:	c3                   	retq   

0000000000000022 <main>:
  22:	55                   	push   %rbp
  23:	48 89 e5             	mov    %rsp,%rbp
  26:	48 83 ec 10          	sub    $0x10,%rsp
  2a:	c7 45 f8 01 00 00 00 	movl   $0x1,-0x8(%rbp)
  31:	8b 15 00 00 00 00    	mov    0x0(%rip),%edx        # 37 <main+0x15>
  37:	8b 05 00 00 00 00    	mov    0x0(%rip),%eax        # 3d <main+0x1b>
  3d:	01 c2                	add    %eax,%edx
  3f:	8b 45 f8             	mov    -0x8(%rbp),%eax
  42:	01 c2                	add    %eax,%edx
  44:	8b 45 fc             	mov    -0x4(%rbp),%eax
  47:	01 d0                	add    %edx,%eax
  49:	89 c7                	mov    %eax,%edi
  4b:	e8 00 00 00 00       	callq  50 <main+0x2e>
  50:	8b 45 f8             	mov    -0x8(%rbp),%eax
  53:	c9                   	leaveq 
  54:	c3                   	retq   

```
对比反汇编结果，可以发现，反汇编的内容和.text的内容是一一对应的．.text段的第一个字节正是0x55,对应反汇编的"push   %rbp"指令.

* #### 数据段和只读数据段
.data段保存的是<b>已经初始化的全局静态变量和已经初始化的局部静态变量</b>，.SimpleSection.c共有两个这样的变量，分别是global_init_var和static_var,这两个变量一共８个字节，所以".data"段的大小为８个字节．这里请回看.data段的内容
```
Contents of section .data:
 0000 54000000 55000000                    T...U...  
```
前4个字节从低到高分别是0x54,0x00,0x00,0x00,这正好是十进制的８４，也就是 global_init_var 的值.
".rodata"段存放的是只读数据,通常时程序里面被const修饰的变量和字符串常量.单独设置".rodata"不仅可以从语义上支持C++的const关键字,而且操作系统在加载的时候可以将".rodata"段的属性映射成只读，这样保证了程序的安全性.

* #### BSS段
.bss段存放的是<b>未初始化的全局变量和未初始化的局部静态变量</b>，.SimpleSection.c共有两个这样的变量，分别是global_uninit_var和static_var2,这两个变量一共８个字节，但".bss"段的大小只有4个字节,实际上只有static_var2存放在.bss段，而global_uninit_var只是一个未定义的"COMMON符号"．这其实跟不同的语言与不同的编译器实现有关，有些编译器会将<b>全局的未初始化变量</b>存放在目标文件的.bss段，有些则不放，只是预留一个<b>未定义的全局变量符号</b>，等后续链接成可执行文件的时候再在.bss段分配空间．
值得一提的是，不同的编译器会作不同程度的优化，下面仅针对gcc7.2.0作一个实验
```
static int x1 = 0;
static int x2 = 1;
```
上述代码中，x1会被放在.bss段，而x2会被放在.data段，这是因为x1为0可以认为是未初始化的，因为未初始化的都是０，所以被优化掉了，这样可以节省磁盘空间．

* #### 其他段
除了上述常用的段以外，下表列出了一些常见的段

|段名|说明|
|----|----|
|.rodata1|跟".rodata"一样|
|.comment|存放的是编译器版本信息|
|.debug|调试信息|
|.dynamic|动态链接信息|
|.hash|符号哈希表|
|.line|调试时的行号表|
|.note|额外的编译信息|
|.strtab|StringTable.字符串表，用于存储ELF文件中用到的各种字符串|
|.symtab|SymbolTable.符号表|
|.shstrab|Section String Table 段名表|
|.plt<br/>.got|动态链接的跳转表和全局入口表|
|.init<br/>.fini|程序初始化与终结代码段|

正常情况下，GCC编译出来的目标文件中，代码和变量等都会被放置到默认的段中，如果希望将代码和变量放到用户自定义的段中，GCC提供了这么一种扩展机制
```
__attribute__((section("FOO"))) int global = 42;
__attribute__((section("FOO"))) void func() {
}
```
如上述代码所示，我们在全局变量或者函数之前加上"__attribute__((section("name")))"属性就可以将相应的变量或者函数放到以"name"作为段名的段中

