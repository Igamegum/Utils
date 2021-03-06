---
title: "浅谈C++中的内存对齐"
tags: [C++]
---

内存对齐一直是一个让我很懵逼的问题，因为会涉及到底层的问题，网上很多的说法都不一致，而且非常依赖于测试的环境，偏偏内存对齐在各大校招的笔试题基本都会考到，所以今天抽空把自己对这部分知识的理解总结一下。
<!--more-->
## 说明
在开始之前有必要说明一下本文的测试环境，因为内存对齐的结果在不同的编译器和系统都会产生不同的结果，所以抛开运行环境的所有讨论都是耍流氓
**系统：WIN7-64**
**编译器:mingw-g++_4.9.3**

下面罗列一下部分类型在上述测试环境下所占的字节数

| 变量类型 | 大小|
| ------:----- |-------:---- |
|char |1|
|char *|4|
|int |4|
|double|8|
|short|2|
|float|4|
|unsigned int|4|
|long|4|
|long long|8|
|unsigned long long|8|

可能有人会对上述的列表有疑问，认为64位的操作系统下，某些类型的变量所占字节数不应该这么小。值得说明的是，并不能直接从sizeof的结果判断系统的位数，因为sizeof的结果还跟编译器有关，编译出来的程序是有 **程序位数**这个说法的，具体可以[戳这](http://www.cnblogs.com/zyl910/archive/2012/02/19/vcis64.html)


##  #pragma pack(n)
先看一段代码
```C++
#include<iostream>

struct A
{
	char a;
	double b;
	int c;
};

int main()
{
	std::cout<<sizeof(A)<<std::endl;
	return 0;
}
```
输出结果
>24 

可以看到，结构体A的大小并不是简单的**1+8+4 = 13**，而是**24**.这是因为每个变量在内存中并不是连续存放的，而是遵循一定的原则，拥有对应的偏移量。
每个编译器(或者说不同的编译环境)都有一个默认的对齐系数，不妨设这个系数为 n ，表示变量遵循n字节对齐的方式。
对于这个n，如果我们想要查看默认值，可以在代码中加入
```C++
#pragma pack(show)   
```
n的值将以warning的形式给出

摘抄一段百度百科的解释。
>n字节对齐就是说变量存放的起始地址的偏移量有两种情况：第一、如果n大于等于该变量所占用的字节数，那么偏移量必须满足默认的对齐方式，第二、如果n小于该变量的类型所占用的字节数，那么偏移量为n的倍数，不用满足默认的对齐方式。结构的总大小也有个约束条件，分下面两种情况：如果n大于所有成员变量类型所占用的字节数，那么结构的总大小必须为占用空间最大的变量占用的空间数的倍数；否则必须为n的倍数。



解释一下，假设对齐系数为n，变量的大小为x
1. n>=x 那么该变量在内存中的偏移量必须为 x的倍数
2. n<x 那么该变量在内存中的偏移量必须为n的倍数
3. 对于结构体而言，假设该结构中 有M个变量，大小分别为 x1,x2....xm，其中最大值为 x_max,那么对于该结构来说，这M个变量遵循完上述1,2的对齐原则之后，如果总的内存大小 不是 min(n,x_max)的倍数，那么就要作内存补齐操作，使得最后该结构总的大小为 min(n,x_max)的倍数

我们可以用以上的原则解释一下上述代码中结构体A的大小 
首先上述运行环境下的对齐系数 **n=8**
变量 a 的偏移量为 0，大小为 1
变量 b 的大小为 8，偏移量为 min(n,sizeof(double)) = 8 的倍数 ，所以是8
变量c 的 大小为 4，偏移量为  min(n,sizeof(int)) = 8 的倍数, 所以是16
到了这一步，整个结构体所占的大小应该是 20 ，但此时大小不满足第三条原则,所以还要作内存补齐，后增4个字节，以达到8的倍数
所以最后结构体A的大小为24

其实，为了便于理解，我们可以使用**offsetof(A,a)**这个宏，用于求结构体中一个成员在该结构体中的偏移量，代码如下

```C++
#include<iostream>
#include<stddef.h>

struct A
{
	char a;
	double b;
	int c;
};

int main()
{

	std::cout<<sizeof(A)<<std::endl;
	std::cout<< offsetof(A,a) <<std::endl;
	std::cout<< offsetof(A,b) <<std::endl;
	std::cout<< offsetof(A,c) <<std::endl;

	return 0;
}

```
输出结果
>24
0
8
16

回到前面，#pragma pack(n) 就是让我们设定编译器以n字节对齐的。
假定上述代码中，我们加入 #pragma pack(4),输出结果则变为
>16
0
4
12

原因这里就不解释了，大家可以作更多的尝试，值得一提的是，参数n的值只能为1、2、4、8、16，否则编译器会按照默认值编译

其实C++11中提供了**alignof** 和**alignas**这两个操作符。分别用于查看变量的对齐边界，以及设定自定义类型的对齐边界。可惜我只知道用法，并没有很深刻理解，这里先占坑不述。
 
## 为什么要内存对齐

内存对齐是一种空间换时间的策略，对齐的数据在读写上会有性能上的优势。更为严重的是，在某些平台上，硬件将无法读取不按照字对齐对齐的某些类型数据，这个时候硬件会抛出异常来终止程序。然而更为普遍的，在一些平台上，数据不按字对齐会造成数据读取效率低下。
假设我们有两个变量 char和 int ，在内存上的分布如图 
![](http://7xvs12.com1.z0.glb.clouddn.com/%E5%86%85%E5%AD%98%E5%AF%B9%E9%BD%90.png)

上方是没有遵循内存对齐，下方则遵循了内存对齐。
假定我们现在要读取该int变量，CPU一次读取4个字节，明显的，上方需要进行两次读入操作，而下方只需要一次！这就是典型的内存对齐在读写性能上的优势。








