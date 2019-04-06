---
title: "C++11追踪返回类型"
tags: [C++]
---

追踪返回类型配合auto 和 decltype 会真正释放C++11中泛型编程的能力！
<!--more-->

## 追踪返回类型的引入

在C++98中,如果一个函数模板的返回类型依赖于实际的入口参数的类型，那么该返回类型在模板实例化之前可能都无法确定,这样的话我们在定义该函数模板的时候就会遇到麻烦。如下代码
```C++
template<typename T1,typename T2>
double Sum(T1 a,T2 b)
{
	auto ans = a + b ;
	std::cout<<"The type of ans is:"<<typeid(ans).name()<<std::endl;
	return ans;
}

```
不管Sum函数中，ans的类型如何，我们都要很憋屈的将结果转成double返回,这里还有可能发生隐式转换的错误，这种情况将大大降低C++泛型编程的能力。
如果你想到利用decltype，你可能会将上述代码修改成如下

```C++
template<typename T1,typename T2>
decltype(a+b) Sum(T1 a,T2 b)
{
	auto ans = a + b ;
	std::cout<<"The type of ans is:"<<typeid(ans).name()<<std::endl;
	return ans;
}
```

很遗憾，这段代码会报未定义的错误,因为编译器是从左往右地读入符号的,在推导**decltype(a+b)**的时候，表达式中的a和b都未声明。
因此，为了解决这个问题,C++11引入新语法——追踪返回类型。上述代码可以修改成如下:


```bash
template<typename T1,typename T2>
auto Sum(T1 a,T2 b) -> decltype(a+b)
{
	auto ans = a + b ;
	std::cout<<"The type of ans is:"<<typeid(ans).name()<<std::endl;
	return ans;
}

```

## 使用追踪返回类型的函数

追踪返回类型的函数和普通函数的声明最大的区别在于返回类型的后置。
追踪返回类型除了能够提高泛型编程的能力，在一定情况下还能提高代码的可读性。

```C++
int  ( * (*pf()) () ) ()
{
	return nullptr;
}
```

不得不说,对于一般人而言,这段代码可读性了相当差，然而我们利用追踪返回类型，可以作如下分解


```bash
// auto (*)() -> int(*)() 一个返回函数指针的函数
// auto PF() ->auto (*)() ->int(*)() 
auto PF()->auto (*)() ->int (*)()
{
	return nullptr;
}

int main()
{

	std::cout<< std::is_same<decltype(PF),decltype(pf)>::value<<std::endl;//1

	return 0;
}
```
改写后的**PF**与**pf**的类型是一样的
对于 **int  ( *(*pf()) () ) ()** 的解释，摘抄某位大神的解释
>pf() pf是一个函数
*pf() 这个函数的返回值是指针
(*pf())() 这个指针是一个函数指针
*(*pf())() 函数指针的返回值还是指针
(*(*pf())())() 这个指针还是函数指针
int (*(*pf())())() 返回值是int

函数可做如下分解

```C++
typedef int(*R)();

typedef R(*R1)();

R1 pf()
{
	return nullptr;
}
```

除了函数模板、普通函数、函数指针、函数应用外，追踪返回类型还可以用在结构或类的成员函数、类模板的成员函数里，方法大同小异，不再赘述。



