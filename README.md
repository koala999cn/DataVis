# DataVis

尝试入坑imgui，很多底层代码需要重构，目前很不成熟，请下载v0.3.1分支代码。当前，已搭建完成基于imgui的程序主体框架，暂时长这样：

![screenshot](screenshots/main.png) 


# 依赖库

| 库名称 | 作用 | 编译 |
|---|---|---|
| [ImGui](https://github.com/ocornut/imgui) | 界面显示 | 已内嵌 |
| [glfw](https://github.com/glfw/glfw) | ImGui依赖 | 单独编译 |
| [imnodes](https://github.com/Nelarius/imnodes) | 节点编辑器 | 已内嵌 |
| [ImFileDialog](https://github.com/dfranx/ImFileDialog) | 文件对话框 | 已内嵌 |
| [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) | ImFileDialog依赖 | 已内嵌 |
| [glad](https://github.com/Dav1dde/glad) | ImFileDialog依赖 | 单独编译 |
| [nlohmann](https://github.com/nlohmann/json) | 解析theme文件 | 已内嵌 |