# white-dew | 白露
<div align="center">
    <img alt="C Badge" src="https://img.shields.io/badge/C-3e4aac?logo=c&logoColor=fff&style=flat"/>
    <!-- <img alt="C++ Badge" src="https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=fff&style=flat"/> -->
    <img alt="CMake Badge" src="https://img.shields.io/badge/CMake-cf3d7e?logo=cmake&logoColor=fff&color=cf3d7e&style=flat"/>
    <img alt="Linux Badge" src="https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=000&color=yellow&style=flat"/>
    <img alt="License Badge" src="https://img.shields.io/github/license/vshowc/white-dew"/>
</div>
<div align="center">
    <img alt="GitHub Repo stars" src="https://img.shields.io/github/stars/vshowc/white-dew?style=social">
    <img alt="GitHub forks" src="https://img.shields.io/github/forks/vshowc/white-dew?style=social">
    <img alt="GitHub watchers" src="https://img.shields.io/github/watchers/vshowc/white-dew?style=social">
</div>


## What is the white-dew | 白露是什么
```textmate
A dynamic memory pool static library written in C language and implemented based on thread local storage technology
```

```textmate
一款由 C 语言编写并基于线程本地存储技术实现的动态内存池静态库
```

## What is the problem white-dew solved | 白露解决了什么问题
```textmate
1. In the case of high concurrency, the frequent opening and release of memory in C language
2. Memory management of C language
```

```textmate
1. 高并发情景下, C/C++ 语言中内存频繁开辟以及释放问题
2. C/C++ 语言内存管理问题
```

## How to quickly start using white-dew | 如何快速开始使用白露
```textmate
1. First, download the corresponding version of the static library compression package file from the released Releases or code release branch.
   Of course, you can also directly download the main branch source code to compile the static library.
       
2. Take the release branch code package as an example.
    
   The file structure is as follows:
               ./white-dew
               ├── headers
               ├── lib
               ├── .gitignore
               ├── LICENSE
               ├── README.md
       
   headers: Place header file directory
   lib: Place the static library file directory
   .gitignore: Git configuration file
   LICENSE: License file
   README.md: Description file
       
3. Import static library files and header files into the project.
    
   Tip: This example is based on C language and CMake scenario.
   (1) Add the static library file in the CMakeLists.txt file. The specific adding method is as follows (only the main configuration items are shown).
       ···
       link_directories(src)                           // src: The absolute path or relative path of the static library file directory
       target_link_libraries(project_name lib_name)    // project_name: Your project name; lib_name: white-dew compiled static library file name
       ··
   (2) Copy the header file in the headers directory to the place where your project places the header file.
    
4. Use white-dew in the project.
    
   (1) Use dy_malloc(size_t _size) replaces the original malloc(size_t _size) to open up memory.
       
       int * a = (int *) malloc(sizeof(int));  ===>   int * a = (int *) dy_malloc(sizeof(int));
   (2) Memory release problem.
       
       · If you manually open a new thread (or the thread has a complete life cycle), you do not need to release memory manually, the callback method will be automatically executed at the end of the thread to free memory.
       · If you use thread pool, you only need to call the dy_free() method at the end of the overall business.
           
       dy_free();
```

```textmate
1. 首先从发布的 Releases 或者代码 release 分支中下载对应版本的静态库压缩包文件, 当然也可以直接下载 main 分支源码自行编译静态库
    
2. 以 release 分支代码包为例
   文件结构如下:
              ./white-dew
              ├── headers
              ├── lib
              ├── .gitignore
              ├── LICENSE
              ├── README.md
                  
   headers: 放置头文件目录
   lib: 放置静态库文件目录
   .gitignore: git 配置文件
   LICENSE: 许可证文件
   README.md: 描述文件
       
3. 在项目中引入静态库文件以及头文件
    
   提示: 此示例基于 C 语言以及 CMake 情景下
   (1) CMakeLists.txt 文件中引入静态库文件, 具体引入方式如下 (只展示主要配置项)
       ···
       link_directories(src)                            // src: 存放静态库文件目录的绝对路径或者相对路径
       target_link_libraries(project_name lib_name)     // project_name: 项目名称; lib_name: white-dew 编译后静态库文件名称
       ···
   (2) 将 headers 中 memory_pool.h 头文件复制到项目放置头文件的地方
       
4. 在项目中使用 white-dew
    
   (1) 用 dy_malloc 代替原始的 malloc 开辟内存
       
       int * a = (int *) malloc(sizeof(int));  ===>   int * a = (int *) dy_malloc(sizeof(int));
           
   (2) 内存释放问题
       
       如果是手动开辟新线程(或者说线程有完整的生命周期), 则不需要手动释放内存, 在线程结束时会自动执行回调方法释放内存
       如果是使用线程池, 则只需要在整体业务结束时调用 dy_free() 方法
           
       dy_free();
```
