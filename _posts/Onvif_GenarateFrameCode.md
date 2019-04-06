---
title: "Windows下生成Onvif框架代码"
tags: [Onvif]
---

## 1. 下载gsoap
到官网下载[gsoap](https://sourceforge.net/projects/gsoap2/files/),我下载的是**2.8.66**版本，下载好之后并解压。
<!--more-->

## 2. 下载WSDL和XSD文件
1. 到官网[下载](https://www.onvif.org/profiles/specifications/),其中每个WSDL相当于Onvif的一个模块，不同的模块对应不同的功能，可以按需下载，部分WSDL文件需要依赖XSD文件，如果不是特别熟悉每个文件对应的功能，也可以全部下载，这样起码能保证需要的功能都能支持。下载方法很简单，直接右键链接，选择*链接另存为*，将文件保存到本地。
2. 将wsdl文件和xsd文件分别放到不同的文件夹下面,因为所有文件都下载到本地了，所以需要修改wsdl文件中依赖的xsd文件的路径。
3. 依次编辑每个WSDL文件，搜索文件中的**schemaLocation**关键字，将XSD文件的路径值修改为本地的XSD文件的路径(推荐使用绝对路径)。

## 3. 生成Onvif文件
1. 打开解压后的gsoap-2.8文件夹，将路径切换至*gsoap-2.8\gsoap\bin\win32*下，将上一步修改好的wsdl文件整个文件夹拷贝到当前路径下
2. 将*gsoap-2.8\gsoap*目录下的**typemap.dat**文件复制到当前文件夹下
![catalog](http://7xvs12.com1.z0.glb.clouddn.com/onvif_catalog_init.png)
3. 在命令行中将路径切换至当前文件夹下，并执行命令
    >wsdl2h -t typemap.dat -o onvif.h ./wsdl/accesscontrol.wsdl ./wsdl/accessrules.wsdl ./wsdl/actionengine.wsdl ./wsdl/advancedsecurity.wsdl ./wsdl/analytics.wsdl ./wsdl/deviceio.wsdl ./wsdl/analyticsdevice.wsdl  ./wsdl/credential.wsdl ./wsdl/devicemgmt.wsdl ./wsdl/display.wsdl ./wsdl/doorcontrol.wsdl ./wsdl./event.wsdl ./wsdl/imaging.wsdl ./wsdl/media.wsdl ./wsdl/ptz.wsdl ./wsdl/receiver.wsdl ./wsdl/recording.wsdl ./wsdl/replay.wsdl  ./wsdl/schedule.wsdl ./wsdl/search.wsdl ./wsdl/thermal.wsdl
4. 如果上一步执行成功，当前文件夹下会生成一个**onvif.h**文件，编译信息如下
    ![onvif_compile](http://7xvs12.com1.z0.glb.clouddn.com/onvif_conpile.png)

## 4. 生成框架代码
1. 编辑onvif.h ,加入代码**#import "wsse.h"**用于鉴权
    ![onvif_wsse](http://7xvs12.com1.z0.glb.clouddn.com/onvif_wsse.png)
2. 执行命令，生成onvif的框架代码
    >soapcpp2 -j -2 -x onvif.h -I../../import -I../../

  关于soapcpp2 的用法参数的含义，可以先执行**soapcpp2 -h**查看
3. 如果上一步执行成功，将会在当前文件夹生成一系列的.cpp、.h、.nsmap文件
    ![onvif_code](http://7xvs12.com1.z0.glb.clouddn.com/onvif_framecode.png)

## 5.写在最后
如果一切执行顺利的话，框架代码已经生成完毕，其中文件名类似于*"xxxBindingService"* 的是服务端的代码，类似于*"xxxBindingProxy"*的是客户端的代码,而.nsmap文件的内容都一样，实际开发中只需要保留一份就够了






