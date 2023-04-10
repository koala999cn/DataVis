# DataVis - 基于ImGui的流数据分析与可视化软件（DSP & Visualization）

## 介绍

DataVis是一款集数据清洗、数据分析、数据渲染于一体的实用软件。重点专注于两方面：一是数据可视化，通过内置主题提供专业化的绘图功能；二是流分析，通过pipeline组合各种功能模块，对各类动态数据提供在线分析和结果演示。
界面如下：

![screenshot](screenshots/main.png) 

## 特性

- 使用ImGui作为界面库，具有跨平台、轻量和高效等优势

- 统一2d/3d绘图接口，并内置opengl实现，支持百万量级数据的实时渲染

- 引入pipeline概念，支持定制复杂的数据处理和渲染流程

- 对高维数据处理和展示友好

- 用户友好，易上手，所有属性项可通过GUI配置

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
| [stb](https://github.com/nothings/stb) | 图片读写支持 | 内联 |
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
| [freetype（可选）](https://freetype.org/) | 字体显示 | 单独编译 |

## 编译

编译器使用VC2019，预编译的64位依赖库可在[附件页面下载](https://gitee.com/koala999/data-vis/attach_files)。

配置imconfig.h中的IMGUI_ENABLE_FREETYPE宏，可启用或禁用freetype库。

