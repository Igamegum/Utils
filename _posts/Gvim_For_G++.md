---
title: "WIN 下Gvim 配置 G++ 编译器"
tags: [VIM,G++]
---

从大三开始就入坑VIM了，作为一名没有任何觉悟的菜鸟，一直都是在Windows下进行开发，所以一直以来用的都是Gvim。G++的配置很早之前就已经实现了，因为感觉最近电脑的硬盘有随时"爆炸"的可能，特意markdown一下自己的配置，起个备忘的作用.
 <!--more-->

## 配置编译器

简单的说就是我们要在WIN上配置好一个编译器，能够在命令行利用它进行cpp或者c文件的编译。
如果系统里面已经相关的IDE了，可以直接将相关IDE的编译器路径配置到环境变量中，比如vistual studio的CL编译器，或者下个自带编译器的CodeBlocks，里面是内置g++的。

因为早前打算学习C++11，需要用到版本比较高的编译器，所以我就直接下载了最新的 [MinGW](http://www.mingw.org/),有点英文基础的很容易就能找到下载链接，或者直接 [戳这](https://sourceforge.net/projects/mingw/files/latest/download?source=files),安装什么的就不用多说了。

安装好了之后，里面还是没有编译器的，我们打开 MinGW Installation Manager,左上角选中Basic Setup,在右边的子窗口选中mingw32-gcc-g++,这个就是我们需要的编译器，然后右键,选中,Mark For Installation ,注意，我这边是已经安装好的，所以图里这一项是不可选的 ![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim_For_G%2B%2B01.png).

然后点击左上方的Installation,再点击Apply Changes，接下来就是等待自动下载的事情了!
![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim_For_G%2B%2B02.png)

如果到这里都没有问题，下载完成后，在MinGW安装目录的bin 文件夹下，应该会发现多了很多.exe和.dll 文件，你也能轻易的找到轻易地找到g++.exe这个程序。接下来把g++所在路径配置到环境变量中，我这边是 C:\MinGW\bin,配置完成后，打开命令行，输入g++ -v，如果显示如下图，则表示配置成功！
![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim_For_G%2B%2B03.png)

## 配置\_vimrc
接下来就是对\_vimrc 的配置，我们分别写一个编译函数和一个运行函数，并设置相应的快捷键，使得能够实现一键编译和一键运行。

对于\_vimrc的配置语法，可以 [戳这](http://blog.chinaunix.net/uid-26707720-id-3329205.html),个人感觉跟shell脚本差不多，毕竟Gvim是从Linux下的Vim而来的。

先不废话，对于G++的配置，我的代码如下
``` bash

"定义CompileRun函数，用来编译代码 
func CompileRun()  
exec "w"  
  
if &filetype == 'c'  
exec "!gcc % -g -o %<.exe"  
  
elseif &filetype == 'cpp'  
exec "!g++ % -g -o %<.exe"  
endif  
endfunc

"定义Run函数  
func Run()  
if &filetype == 'c' || &filetype == 'cpp'  
exec "!%<.exe"  
endif  
endfunc  


map <F9> :call CompileRun()<CR>  
map <F10> :call Run()<CR>  
```

代码还是很好理解的

> exec "w"

这句起的是保存的作用,相当于在Normal 模式下输入:w

> exec "!%<.exe"

这句话是执行跟文件同名的.exe文件,效果相当于在命令行执行该.exe文件

> exec "!g++ % -g -o %<.exe"  

这句话理解起来跟上面是一样的，相当于在命令行下用g++编译文件，至于 -g -o 是编译选项,熟悉g++编译的一定知道。

> map &lt;F9&gt; :call CompileRun() &lt;CR&gt;

这里只是作一个执行相应函数的快捷键配置，不多说了。


## 总结
1. 将相应的编译器配置到环境变量中
2. 在\_vimrc书写相应的语法

整个过程并不复杂,VIM的轻量级对比IDE一直是个优势，配置好之后，VIM能更近一步的代替IDE了!










