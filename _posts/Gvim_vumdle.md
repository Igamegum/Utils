---
title: "WIN下Gvim管理插件"
tags: [VIM]
---

Vim的插件手动安装是比较繁琐的，这里介绍一个专门管理Vim插件的插件——**Vundle**。使用Vundle可以很方便的实现插件的安装和删除，省去了大量手工操作的麻烦！

<!--more-->

## Git的安装
如果Win上已经装好了Git的可以直接跳过这一步
这里是[官方下载链接](https://git-scm.com/downloads)，或者可以[戳这](https://git-for-windows.github.io/)。下载好之后，安装什么的就不需要多说了，直接全点下一步就OK！
**值得一提的是，需要将git.exe所在的文件目录配置到环境变量中**
以我为例，我这边是 
>C:\Program Files (x86)\Git\bin

配置好之后，打开命令行，输入 git --version，如果显示如下图，说明第一步成功<br/>
![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim-bundle_04.png)

## 安装Vundle
打开Vim安装所在的目录,在vimfiles目录下，新建目录Bundle,并且打开命令行，切换到该目录<br/>
![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim-bundle_06.png)

输入
>git clone https://github.com/gmarik/vundle

等待下载

![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim-bundle_05.png)

下载成功后，需要配置\_vimrc

在\_vimrc中加入以下代码

```bash
"Vundle的路径
set rtp+=$VIM/vimfiles/bundle/vundle/
"插件的安装路径
call vundle#begin('$VIM/vimfiles/Bundle/') 
Bundle 'gmarik/vundle'  
call vundle#end()
filetype plugin indent on 
```
以后添加插的代码就直接写在
**call vundle#begin()** 和 **callvundle#end()** 之间

重新打开Vim，在Normal模式下输入 **:BundleList**,即可查看当前\_vimrc文件中指定的插件<br/>
![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim-bundle_02.png)<br/>
再在Normal模式下输入**:BundleInstall**，后台便会自动下载并安装插件栏里还未安装的插件,当状态栏里显示Done！的时候，表明所有插件都安装完毕。
其实到这里为止，你便可以根据自己的需要，自由地安装插件了！

下面再讲解一下vim-airLine插件的安装
## 安装 vim-airLine
airLine简单的说就是vim状态栏的强化版，当然，还添加了很多其他的功能

我们先在\_vimrc中vundle的插件配置中添加一句

>Bundle 'bling/vim-airline'

![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim-bundle_10.png)

然后再添加以下代码作配置

```bash

 "打开tabline功能,方便查看Buffer和切换，这个功能比较不错"

 let g:airline#extensions#tabline#enabled = 1
 let g:airline#extensions#tabline#buffer_nr_show = 1


 " 关闭状态显示空白符号计数"
 let g:airline#extensions#whitespace#enabled = 0
 let g:airline#extensions#whitespace#symbol = '!'


 " 设置consolas字体"
 set guifont=Consolas\ for\ Powerline\ FixedD:h11

 "设置状态栏符号显示，下面编码用双引号"
 let g:Powerline_symbols="fancy"
 let g:airline_symbols = {}
 "let g:airline_left_sep = "\u2b80" 
 let g:airline_left_sep = "\u25b6" 
 let g:airline_left_alt_sep = "\u2b81"
 "let g:airline_right_sep = "\u2b82"
 let g:airline_right_sep = "\u25c0"
 let g:airline_right_alt_sep = "\u2b83"
 let g:airline_symbols.branch = "\u2b60"
 let g:airline_symbols.readonly = "\u2b64"
 let g:airline_symbols.linenr = "\u2b61"

 "设置顶部tabline栏符号显示"
 let g:airline#extensions#tabline#left_sep = "\u25b6"
 let g:airline#extensions#tabline#left_alt_sep = "\u2b81"

```

保存后打开vim，输入**:BundleInstall**完成插件的安装。
重新打开VIM,效果如图

![](http://7xvs12.com1.z0.glb.clouddn.com/Gvim-bundle_01.png)
## 删除插件
插件的删除也方便，直接将\_vimrc中的插件配置语句注释掉,比如我要删除刚才装的airline,只需要注释掉 **Bundle 'bling/vim-airline'**。
重新打开vim,输入**BundleClean**,提示之后输入'Y'确定，便能一键删除插件。

## 总结
1. 安装git。
这是必不可少的一步，后续vim插件的安装基本都是通过git作下载的。<br/>
2. 安装Vundle。
Vundle下载之后，再添加相关的配置代码。便能实现一键安装、删除、搜索插件了！










