---
title: "C++11正则表达式"
tags: [C++,正则表达式]
---
上班的时候抽空学了一下 C++ 正则表达式的使用方法,在此作个小结！

<!--more-->


###### 编译需加选项 -std=c++11

## 1.std::regex_match
[参考文档](http://www.cplusplus.com/reference/regex/regex_match/)
###   &#160; &#160;1.1std::regex_match(std::string,std::regex)
  &#160; &#160;&#160;&#160;**string** 成功 匹配 **regex** 返回 true,其他情况返回false

###   &#160; &#160;1.2std::regex_match(std::string,std::match_results<string::const_iterator>,std::regex)
 &#160; &#160;&#160;&#160;首先整个**string**要匹配**regex**,其次std::match_results<string::const_iterator>中依次存储**regex**中的子表达式的匹配子串(空串也是子串)

```C++
   	std::string pattern = "(\\d{3})(\\d{2})";
		std::regex e(pattern);

		std::string str = "12345";

		std::smatch sm;
		std::regex_match(str,sm,e);

		for(int i=0;i<sm.size();i++){
				std::cout<<sm[i]<<std::endl;
		}
	
```
###### 输出结果

>12345
>123
>&nbsp; 
>45



## 2.std::regex_search
[参考文档](http://www.cplusplus.com/reference/regex/regex_search/)
###   &#160; &#160;2.1std::regex_search(std::string,std::match_results<string::const_iterator>,std::regex)
 &#160; &#160;&#160;&#160;regex_search 只要**string**中存在符合**regex**的子串就返回结果，而regex_match要求整个**string**符合**regex**

```C++
        std::string pattern = "(\\d{3})(\\d{2})";
        std::regex e(pattern);
        std::string str = "12345 efg 05486 e57 7565";
        std::smatch sm; // same as std::match_results<string::const_iterator> sm;
          while (std::regex_search (str,sm,e)) {
           for (auto x:sm) std::cout << x << " ";
               std::cout << std::endl;
               str = sm.suffix().str();
          }

```
###### 输出结果

>12345 123 45
>05486 054 86



## 3.std::regex_replace
[参考文档](http://www.cplusplus.com/reference/regex/regex_replace/)
###   &#160; &#160;3.1std::regex_replace(std::string,std::regex,std::string)
&#160; &#160;&#160;&#160;将**string1**中，符合**regex**的子串用**string2**替换

```C++
		std::string pattern = "(w)(e)(m)(ade)(f)(ox)";
		std::regex e(pattern);

		std::string str = "wemadefox";

		std::string replacer = "W$2M$4F$6";

		std::cout<<std::regex_replace(str, e, replacer)<<std::endl;
```
&#160; &#160;&#160;&#160;代码中**replacer**的 **$2** 代表 **regex**中的第二个子表达式

###### 输出结果

>WeMadeFox



###   &#160; &#160;3.2std::regex_replace(std::iterator<string>,std::iterator<string>,std::iterator<string>,std::regex,std::string)
&#160; &#160;&#160;&#160;与上面一个函数的区别它支持范围性的匹配，并且能将匹配结果存储到迭代器指向的内存中

```C++
 	std::string pattern = "(m)(ade)(f)(ox)";
		std::regex e(pattern);

		std::string str = "wemadefox";

		std::string replacer = "M$2F$4";

		std::string result;
		
		std::regex_replace(std::back_inserter(result),str.begin()+2,str.end() ,e, replacer);

		std::cout<<result<<std::endl;
```

###### 输出结果

>MadeFox



