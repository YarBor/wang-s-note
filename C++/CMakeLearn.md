## CMake 
通过cmake对源码依赖进行分析设定 生成makefile
通过makefile进行编译

linux下使用cmake生成makefile的流程如下
1. 编写CMake的配置文件CMakeList.txt
2. 执行`cmake <path>`生成makefile
3. 使用make进行编译

## 语法 
- 指定cmake最小版本 
`cmake_minimum_required(VERSION 3.4.1)`
- 设置项目名称
`project(demo)` 
它会引入两个变量 demo_BINARY_DIR 和 demo_SOURCE_DIR，同时，cmake 自动定义了两个等价的变量 PROJECT_BINARY_DIR 和 PROJECT_SOURCE_DIR
- 设置编译类型
  ```CMakeList.txt
  add_executable(demo demo.cpp) #-->可执行文件
  add_library(common STATIC util.cpp) # --> 生成静态库
  add_library(common SHARED util.cpp) # --> 生成动态库或共享库
  ```
  add_library 默认生成是静态库，通过以上命令生成文件名字
  linux-> `demo libcommon.o / libcommon.so`
  win -> `demo.exe common.lib common.dll`
- 