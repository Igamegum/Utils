---
title: "C++11移动语义与完美转发"
tags: [C++]
---
 移动语义与完美转发都是是C++11新增加的两个特性，或者说都是由右值引用这个特性而来，要理解右值引用，要先从C++的值类型谈起。
<!--more-->
## 左值、纯右值、将亡值
在C++98中只有左值和右值之分，而C++11中引入了将亡值这个类型，在C++11中，右值由纯右值和将亡值组成，纯右值就是C++98标准中右值的概念,而将亡值是C++11新增的跟右值引用相关的表达式。

我们先了解怎么区分左值和右值。左值是指示表达式结束后依然存在的持久化对象，右值是指表达式结束时就不在存在的临时对象。通俗地说，区分左值和右值的方法就是看能不能对表达式取地址，如果能，则为左值，否则为右值。
所有的具名变量或对象都是左值，而右值是不具名的。

举个例子
```C++
a = b + c;
```
在这个赋值表达式中,a就是一个左值，而b+c是一个右值,因为a是可以取地址的,&a是合法的，而&(b+c)是不合法的，因为 b+c 是不具名的，在上述表达式中返回的只是一个临时对象。
在C++11中，非引用返回的临时标量、运算表达式产生的临时标量、原始字面量和lambda表达式等都是纯右值。例如 
```C++
1)    1+3
2)    'A'
3)    [](int x)->int{return x*x;}
```
上述3个例子都是纯右值

之前说到将亡值是跟右值引用相关的表达式，这样的表达式通常是将要被移动的对象，比如返回右值引用T&&的函数返回值、std::move的返回值、或者转换为T&&的类型转换函数的返回值。

**在C++11程序中，所有的值必属于左值、将亡值、纯右值三者之一。**

详细的文档可以参考[这里](http://en.cppreference.com/w/cpp/language/value_category) 
到这里为止，如果你对左值和右值有一个大致的概念，便足够了。

## 移动语义
让我们,先看一段代码
```C++
#include<iostream>

class A
{
		public:
		A():ptr(new int(0)) {std::cout<<"construct: "<<++num_cs<<std::endl;}
		A(const A & temp):ptr(new int(*temp.ptr)) {
				std::cout<<"copy construct: "<<++num_cp<<std::endl;
		}
		~A() { delete(ptr); std::cout<<"destruct: "<<++num_ds<<std::endl; }
		int *ptr;
		static int num_cs;
		static int num_ds;
		static int num_cp;
};

int A::num_cs = 0;
int A::num_ds = 0;
int A::num_cp = 0;

A GetA() {return A();}

int main()
{
		A var = GetA();
		return 0;
}
```
**上述代码在编译的时候需要加上选项  -fno-elide-constructors **

简单说明一下，这个选项是用来关闭 编译器的 **RVO**，就是返回值优化。然而编译器的优化并不在C++的标准里面，关掉之后我们才能清楚看到未被编译器优化的代码是如何执行的。（这个优化是默认开启的，不同的编译器可能优化效果不一样）

###### 输出结果
>construct: 1
copy construct: 1
destruct: 1
copy construct: 2
destruct: 2
destruct: 3

让我们分析一下输出结果
前三句都是发生在函数GetA()函数里面的
A()调用了一次构造函数,对应 construct: 1
GetA()需要产生临时对象作为函数返回值的时候，调用了一次拷贝构造函数,对应copy construct: 1
GetA()函数结束，构造的对象被销毁,对应 destruct: 1
A a = GetA();将临时对象赋值给a 调用一次拷贝构造函数 ，对应 copy construct: 2
A a = GetA();语句结束，临时对象销毁，对应 destruct: 2
main()函数结束，销毁对象a 对应 destruct: 3

我们可以看到，类A的拷贝构造函数中，指针 ptr 申请了一块跟 被拷贝对象 temp 的指针 ptr 同样大小的内存,而很多时候，被拷贝对象只是作为临时变量，如果类中的指针指向一块非常大的内存，临时变量的构造、拷贝、销毁都需要对这块内存作申请和释放的操作，这种频繁的操作将极大地拖慢程序的运行速度。
如图

![](http://7xvs12.com1.z0.glb.clouddn.com/%E6%8B%B7%E8%B4%9D%E6%9E%84%E9%80%A0.png)

拷贝构造的时候，var申请了一块跟临时变量同样大小的内存，拷贝构造完成后，释放了临时变量的内存

一个很直观的想法就是，在初始化var的时候,能不能不申请内存，而直接将临时变量已经申请的内存挪给var。
也许你会觉得只需要改动一下拷贝构造函数的代码，让var和temp的指针指向同一块内存,很遗憾，如果temp被析构,var所指向的就是一块被释放的内存，这就是经典的"指针悬挂"的错误。也许你还会再加一句，让temp的指针置为空指针，这样temp被析构的时候就不会释放堆内存了，别忘了，拷贝构造函数的原型是

**ClassName(const ClassName & T)**

temp是一个常引用，是只读的
为了解决上面的问题呢，C++11里面引入了移动构造函数
如图

![](http://7xvs12.com1.z0.glb.clouddn.com/%E7%A7%BB%E5%8A%A8%E6%9E%84%E9%80%A0.png)

基本思想上面已经描述过了，就是var初始化的时候，将指针指向跟temp相同的一块内存，然后将temp的指针置为空指针。
有了移动构造函数，我们可以将上述代码改成如下

```C++
#include<iostream>

class A
{
		public:
		A():ptr(new int(0)) {std::cout<<"construct: "<<++num_cs<<std::endl;}
				
		A(const A & temp):ptr(new int(*temp.ptr)) {
				std::cout<<"copy construct: "<<++num_cp<<std::endl;
		}
		A(A && temp):ptr(temp.ptr){
				temp.ptr=nullptr;
				std::cout<<"move construct: "<<++num_mc<<std::endl;
		}
		~A() { 
				delete(ptr);
				std::cout<<"destruct: "<<++num_ds<<std::endl;
		}
		int *ptr;
		static int num_cs;
		static int num_ds;
		static int num_cp;
		static int num_mc;
};

int A::num_cs = 0;
int A::num_ds = 0;
int A::num_cp = 0;
int A::num_mc = 0;

A GetA() {
		A temp;
		std::cout<<"Memory form  "<<__func__<<":"<<temp.ptr<<std::endl;
		return temp;
}

int main()
{
		A var = GetA();
		std::cout<<"Memory form  "<<__func__<<":"<<var.ptr<<std::endl;
	
		return 0;
}
```
###### 输出结果
>construct: 1
Memory form  GetA:0x652df0
move construct: 1
destruct: 1
move construct: 2
destruct: 2
Memory form  main:0x652df0
destruct: 3

对比输出结果，我们发现之前需要由拷贝构造函数执行的地方被移动构造函数所代替。并且var跟临时变量所指向的是同一块内存。

这里还有一个问题，移动构造函数什么时候会被触发。比如我们将上述代码的主函数部分稍作修改
```C++
int main()
{
//        A var = GetA();
//      std::cout<<"Memory form  "<<__func__<<":"<<var.ptr<<std::endl;
  	 	 A a;
		 A b(a);

        return 0;
}
```
###### 输出结果
>construct: 1
copy construct: 1
destruct: 1
destruct: 2

可以发现，这里并没有调用移动构造函数,因为构造对象b的时候，对象a是一个左值。
对于代码
```C++
T(T && var)
```
移动构造函数的触发当且仅当var为将亡值。


## std::move

std::move 是C++11标准库<utility>中的一个函数，它的作用是**将一个左值强制转化为右值引用**，从实现上讲，相当一个类型转换

```C++
static_cast<T&&>(lvalue)
```
需要注意的是，被转化的左值，其生命周期不会因为转化而改变。例如

```bash
#include<iostream>
#include<type_traits>
#include<utility>

class A
{
	public:
	A():ptr(new int(0)){}
	A(const A& temp):ptr(new int(*temp.ptr)){}
	A(A&& temp):ptr(temp.ptr){
		temp.ptr = nullptr;
	}
	int *ptr;
};

int main()
{
		A a;
		A b(std::move(a));//调用移动构造函数
		std::cout<<*a.ptr<<std::endl;//运行出错

		return 0;
}
```

## 右值引用
在C++11中，右值引用就是对一个右值进行引用的类型。比如
```C++
T && a = ReturnValue();
```
这里假设ReturnValue() 返回的是一个右值，那么我们就声明了一个名为a的右值引用，其值等于ReturnValue()返回的临时变量的值
本来ReturnValue()返回的临时变量在语句结束后，其生命周期就结束了，但是通过右值引用的声明，其的生命周期与变量 a 一样，只要 a还“活着“，该临时变量就一直”存活”下去，所以可以说右值引用是为右值“续命”。
所以，相比于以下语句的声明方式
```C++
T a = ReturnTValue();
```
就会少了一次构造函数和析构函数，我们可以利用这个特点，作一些性能上的优化

值得指出的是，右值引用是不能够绑定到任何左值的，比如以下代码会编译出错
```C++
	int lhs;
	int && rhs =lhs;
```
然而常量左值引用却可以绑定到右值，比如
```C++
const int &a = 10;
int &a = 10;
```
上面一行代码是可以编译通过的，下面的却会编译出错，因为常量左值引用在C++98标准中就是个“万能”的引用类型。它可以接受非常量左值、常量左值、非常量右值、常量右值对其进行初始化。

其实从C++98开始，常常利用常量左值引用来减少临时对象的开销
比如
```C++
#include<iostream>

class A
{
		public:
		A(){}
		A(const A & a){std::cout<<"copy"<<std::endl;}
};

void FunVal(A ){std::cout<<__func__<<std::endl;}
void FunRef(const A &){std::cout<<__func__<<std::endl;}
int main()
{
		A a;
		FunVal(a);
		FunRef(a);
		return 0;
}
```
###### 输出结果
>copy
FunVal
FunRef

可以看到，使用常量左值引用作为函数的形参，能够避免对临时对象的拷贝。

在C++11中，各种类型可以引用的值的类型总结如下

| 引用类型 | 非常量左值|常量左值|非常量右值|常量右值|
| ------------ | :--------------|------------|---------------|:-----------|
|非常量左值引用|Y|N|N|N|
|常量左值引用|Y|Y|Y|Y|
|非常量右值引用|N|N|Y|N|
|常量右值引用|N|N|Y|Y|

有时候，我们可能不知道一个类型是否是引用类型，以及是左值引用还是右值引用。标准库<type_traits>头文件中提供了3个类模板:is_rvalue_reference、is_lvalue_reference、is_reference，当我们想要搞清引用类型的时候，不妨用这几个函数验证一下。

## 引用折叠

实际上，T&& 并不一定表示右值，这要根据它绑定的类型区分，比如：
```C++
#include<iostream>
#include<type_traits>

template<typename T>
void fun(T && param)
{
	std::cout<<std::is_reference<T &&>::value<<std::endl;
	std::cout<<std::is_lvalue_reference<T &&>::value<<std::endl;
	std::cout<<std::is_rvalue_reference<T &&>::value<<std::endl;
}

int main()
{
	
		fun(10); //右值
		int x = 10;
		fun(x);//左值
		return 0;
}

```
###### 输出结果
>1
0
1
1
1
0

上面的例子中param 实际上一开始是一个未定义的引用类型，称为 universal references,可以明显地看出,如果被左值初始化，它就是一个左值引用，如果它被一个右值初始化，它就是一个右值引用。这种情况，只有当发生自动类型推断的时候才有效。换句话说，只有会发生类型推断（比如函数模板类型的自动推导，auto关键字的自动推导）时，param在初始化前才是 universal references。
这里的情况比较复杂，我们多看几个例子
```C++
template <typename T>
void f(T && param);//这里T的类型需要推导,所以 T && 是一个universal references

template<typename T>
class Test{
	Test(Test && rhs);//已经定义了一个确定的类型,没有类型推断,T&&是一个右值引用
};
void f(Test &&param);//已经定义了一个确定的类型,没有类型推断,是一个右值引用
```
再看一个复杂点的例子

```C++
template<typename T>
void f(std::vector<T>&& param);
```
这是一个右值引用类型，因为在调用这个函数之前，这个vector<T>中的推断类型已经确定，所以调用函数的时候已经没有了类型推断，再看：
```C++
template<typename T>
void f(const T&& param);
```
这也是一个右值引用类型，因为universal references仅在T&&下发生,任何一点附加条件都会使之失效，所以T&&在被const修饰之后，就变成了右值引用了。

之所以T&& 的类型会根据绑定的值被推导成不同的类型，是因为C++11中引入了自动折叠规则。

举个例子：
```C++
typedef const int T;
typedef T &TR;
TR &v = 1;//C++98中编译出错
```
TR &v = 1;这样的语句一旦在C++11中出现，就会发生引用折叠，具体如下表

|TR的类型定义|声明v的类型|v的实际类型|
| ------------ | :--------------|:-----------|
|T&|TR|A&|
|T&|TR&|A&|
|T&|TR&&|A&|
|T&&|TR|A&&|
|T&&|TR&|A&|
|T&&|TR&&|A&&|

这个表有兴趣的可以通过类模板is_rvalue_reference、is_lvalue_reference自行求证。
表格中的规律很好总结，**一旦出现左值引用，引用折叠总是将其折叠为左值引用。**
而模板对于类型的推导规则比较简单，当实参的是类型X的一个左值引用，那么模板参数被推导为X&类型，而当实参是类型X的一个右值引用，模板的参数被推导为X&&类型。结合以上规则，我们可以实现函数模板的完美转发。

## 完美转发
所谓完美转发，是指函数模板中，完全依照模板的参数类型，将参数传递给函数模板中调用的另外一个函数，比如：
```C++
template<typename T>
void Turning(T t){ Run(t);}
```
这里Turning是一个转发函数模板,而Run才是真正执行实际代码的目标函数。
对于上面的例子，传入t的时候，虽然类型是正确的，但是产生了一次额外的临时对象的拷贝，造成不必要的性能损失。因此这样的转发只能说是正确转发，谈不上完美。所以我们的转发函数和目标函数都需要一个引用类型，引用类型不会有拷贝的开销。
但是我们需要兼顾转发函数和目标函数对参数的接受能力。因为转发函数有可能接受左值或者右值，而目标函数也一样。
因此，你可能会想到转发函数使用“万能”的常量左值引用类型，但是目标函数很多情况下都不能接收常量左值引用(并不是语法上的问题)，因为常量引用的值是不可修改的，很多时候这不符合程序员的设计。

让我们看了例子：
```C++
void Run(int &t){std::cout<<t<<std::endl;}
template<typename T>
void Turning(const T& t){ Run(t);}

int main()
{
	int x = 0;	
	Turning(x);

	return 0;
}
```
这段代码会编译出错，因为目标函数Run 的参数是 非常量引用,而非常量引用是不可以跟常量引用绑定的(可以参考上文的表格)。
然而，为了解决兼顾转发函数和目标函数参数接受能力的问题，我们可以利用右值引用的引用折叠规则解决，将转发函数设计成如下形式 

```C++
void Turning(T &&t){
	Run(static_cast<T &&>(t));
}
```
如果转发函数接受的是一个类型为X的左值引用，那么转发函数被实例化成如下形式
```C++
void Turning(X& &&t){
	Run(static_cast<X& &&>(t));
}
```
应用引用折叠规则之后就是
```C++
void Turning(X& t){
	Run(static_cast<X& >(t));
}
```

如果转发函数接受的是一个类型为X的右值引用，那么转发函数被实例化成如下形式
```C++
void Turning(X&& &&t){
	Run(static_cast<X&& &&>(t));
}
```
应用引用折叠规则之后就是
```C++
void Turning(X&& t){
	Run(static_cast<X&& >(t));
}
```
这样设计的转发函数，无论接受的是参数是左值还是右值，都能无需对临时变量作拷贝地转发给目标函数，并且传递过去的是非常量的引用，而不管目标函数对应的参数是常量引用还是非常量引用，都能跟传递过来的参数进行绑定(可以回看上文的表格)

在C++11中，我们不需要手动地为转发函数进行类型转换,有这么一个用于完美转发的函数叫做： **forward**。所以我们可以把转发函数写成如下形式
```C++
template<typename T>
void PerfectForward(T &&t) {  Run( std::forward<T>(t) ); }
```
我们来看一个例子:
```C++
void Run(int &&m) {std::cout<<"rvalue_reference"<<std::endl;}
void Run(int &m) {std::cout<<"lvalue_reference"<<std::endl;}
void Run(const int &&m) {std::cout<<"const rvalue_reference"<<std::endl;}
void Run(const int &m) {std::cout<<"const lvalue_reference"<<std::endl;}

template<typename T>
void PerfectForward(T &&t) {  Run( std::forward<T>(t) ); }
int main()
{
	int a,b;
	const int c = 1;
	const int d = 0;

	PerfectForward(a);
	PerfectForward(std::move(b));
	PerfectForward(c);
	PerfectForward(std::move(d));

	return 0;
}
```
###### 输出结果
>lvalue_reference
rvalue_reference
const lvalue_reference
const rvalue_reference

可以看到，所有转发都被正确地送到了目的地。
完美转发减少了函数版本的重复，并且充分利用了移动语义。无论从运行性能的提高还是从代码编写的简化上，完美转发都堪称完美。

## 后记
第一次写了一篇这么长的文章，本来只是想把这几天的内容抽一部分出来作个总结，但是这里面涉及到的概念确实比较多，加上自己的理解能力有限，所以整篇文章下来，自我感觉逻辑不是很清晰，很多细节都没有处理好，希望以后能有更深的理解，对文章再作编辑。另外，由于理解不足甚至出现偏差，如果发现文中有错误的地方，欢迎指正！



