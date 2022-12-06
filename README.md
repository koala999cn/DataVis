# DataVis

## 介绍

主要用于流数据的实时分析与显示。重点专注于两方面：一是数据可视化，通过内置主题提供专业化的绘图功能；二是流分析，通过pipeline组合各种功能模块，对各类动态数据提供在线分析和结果演示。
界面如下：

![screenshot](screenshots/main.png) 

## 新特性

v0.4版对底层代码作了较大重构，主要有4方面：

- 使用ImGui作为界面库，不再依赖Qt

- 统一2d/3d绘图接口并内置实现，不再依赖外部库

- 使用节点编辑器构造网状拓扑结构，不再使用树形结构

- 全面引入pipeline概念，支持复杂的数据流程

目前，已基本重构0.3版功能。

## 使用

主要分3步：

一是构造pipeline，通过ActionPanel新增provider、operator和renderer节点，在NodeEditor中建立拓扑链接；

二是配置pipeline，选中单个节点，在PropertySheet中进行参数设定，部分参数可也后期调整；

三是运行pipeline，点击主菜单pipeline的start选项，renderer节点会渲染输出。双击节点可查看该节点的即时数据。

## 依赖库

| 库名称 | 作用 | 编译 |
|---|---|---|
| [ImGui](https://github.com/ocornut/imgui) | 界面显示 | 内联 |
| [glfw](https://github.com/glfw/glfw) | ImGui依赖 | 单独编译 |
| [imnodes](https://github.com/Nelarius/imnodes) | 节点编辑器 | 内联 |
| [ImFileDialog](https://github.com/dfranx/ImFileDialog) | 文件对话框 | 内联 |
| [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) | ImFileDialog依赖 | 内联 |
| [glad](https://github.com/Dav1dde/glad) | ImFileDialog依赖 | 单独编译 |
| [nlohmann](https://github.com/nlohmann/json) | 解析theme文件 | 内联 |
| [blitz](https://github.com/blitzpp/blitz/) | 多维数组支持 | 内联 |
| [praat](https://www.fon.hum.uva.nl/praat/) | FFT | 内联 |
| [readerwriterqueue](https://github.com/cameron314/readerwriterqueue/) | 线程安全队列 | 内联 |
| [rtaudio](http://www.music.mcgill.ca/~gary/rtaudio/) | Audio录放 | 单独编译 |
| [exprtk](http://www.partow.net/programming/exprtk/) | 数学表达式 | 内嵌子项目 |
| [kfr](https://www.kfr.dev/) | 滤波器支持 | 内联 |
| [kGraph](https://gitee.com/koala999/kgl) | pipeline和模块加载 | 内联 |
| [libsndfile](http://libsndfile.github.io/libsndfile/) | 音频文件读写 | 内嵌子项目 |

## 编译

编译器使用VC2019，预编译的64位依赖库可在[附件页面下载](https://gitee.com/koala999/data-vis/attach_files)。