---
title: "如何从WIN7+ubuntu双系统中卸载ubuntu"
tags: [ubuntu]
---

心血来潮感觉电脑上的ubuntu用着有点不爽(迷)，于是打算将原来的ubuntu卸载掉重新安装一个。
先说明一下，我电脑上的环境是WIN7+Ubuntu双系统，并且之前安装ubuntu的时候是用ubuntu引导的WIN，这种情况卸载起来比较麻烦，不是直接在WIN上格式化ubuntu的分区就能完事，因为之前安装ubuntu的时候，将GRUB写入到了MBR（Main Boot Record）。
<!--more-->

怎么判断是不是这种情况呢？就是**每次开机的时候，先进入一个紫色的界面，出现GRUB菜单，然后默认ubuntu选项是在最上方**（当然这种说法并不准确），大概如下图
![](http://7xvs12.com1.z0.glb.clouddn.com/GRUB.jpg)


### 修复MBR
接下来第一步就是下载[MbrFix](http://www.sysint.no/mbrfix)将下载下来的ZIP解压出来，然后命令行进入**MbrFix.exe**所在的文件夹，
输入
```C++
MbrFix /drive 0 fixmbr /yes
```
64位系统的可以输入
```C++
MbrFix64 /drive 0 fixmbr /yes
```
(建议在此操作前，先右键.exe，选择以管理员身份运行)
以我的为例
![](http://7xvs12.com1.z0.glb.clouddn.com/mbrfix.png)

按下Enter键后，如果没有报任何消息，MBR就修复完成了

接下来重启计算机，你就会神奇地发现直接进入WIN系统了。
如果到这一步都很顺利，那么接下来只需要删除ubuntu的分区即可，如果你很不幸地发现和上文所描述的情况不一致，请不要继续接下来的步骤，直接咨询你信任得过的大神！


### 删除分区
在WIN7中删除分区就很简单了。
计算机(右键)→管理→磁盘管理

点击ubuntu所在的分区(可能2个或者4个，取决于之前的安装方法)，**右键→删除卷**(千万不要把原本WIN7中的分区给删除了)
![](http://7xvs12.com1.z0.glb.clouddn.com/%E5%88%A0%E9%99%A4%E5%88%86%E5%8C%BA.png)

删除成功后，我们会看到磁盘卷中多出一块绿色的可用空间，这部分的空间是未被系统所使用的
![](http://7xvs12.com1.z0.glb.clouddn.com/%E5%88%A0%E9%99%A4%E5%88%86%E5%8C%BA%E5%90%8E.png)
当然，如果想把这块内存分配器其他盘符，方法也非常简单，请自行搜索。不过在这里，我会把这块空间留下来安装新的ubuntu系统。

到这里为止，双系统的ubuntu就算卸载完成了。有删必有装，接下来讲解一下怎么安装ubuntu双系统！



