# DataVis

### 介绍
用于流数据的实时分析与显示。主要专注于两方面：一是数据可视化，通过内置主题提供专业化的绘图功能；二是流分析，通过pipeline组合各种功能模块，对各类动态数据提供在线分析和结果演示。

### 编译
Qt6.2.2 + MSVC2019

### 主题
目前，已实现ggthemr所有plotting主题，各主题样例如下。详细可参考[ggthemr开源页面](https://github.com/Mikata-Project/ggthemr)。

#### camouflage

| ![散点图](screenshots/camouflage-scatter.png) | ![点线图](screenshots/camouflage-line-scatter.png) | ![点线图](screenshots/camouflage-bar.png) |
|---|---|---|

#### carrot

| ![散点图](screenshots/carrot-scatter.png) | ![点线图](screenshots/carrot-line-scatter.png) | ![点线图](screenshots/carrot-bar.png) |
|---|---|---|

#### chalk

| ![散点图](screenshots/chalk-scatter.png) | ![点线图](screenshots/chalk-line-scatter.png) | ![点线图](screenshots/chalk-bar.png) |
|---|---|---|

#### citrus

| ![散点图](screenshots/citrus-scatter.png) | ![点线图](screenshots/citrus-line-scatter.png) | ![点线图](screenshots/citrus-bar.png) |
|---|---|---|

#### copper

| ![散点图](screenshots/copper-scatter.png) | ![点线图](screenshots/copper-line-scatter.png) | ![点线图](screenshots/copper-bar.png) |
|---|---|---|

#### deadpool

| ![散点图](screenshots/deadpool-scatter.png) | ![点线图](screenshots/deadpool-line-scatter.png) | ![点线图](screenshots/deadpool-bar.png) |
|---|---|---|

#### dust

| ![散点图](screenshots/dust-scatter.png) | ![点线图](screenshots/dust-line-scatter.png) | ![点线图](screenshots/dust-bar.png) |
|---|---|---|

#### earth

| ![散点图](screenshots/earth-scatter.png) | ![点线图](screenshots/earth-line-scatter.png) | ![点线图](screenshots/earth-bar.png) |
|---|---|---|

#### flat dark

| ![散点图](screenshots/flat_dark-scatter.png) | ![点线图](screenshots/flat_dark-line-scatter.png) | ![点线图](screenshots/flat_dark-bar.png) |
|---|---|---|

#### flat

| ![散点图](screenshots/flat-scatter.png) | ![点线图](screenshots/flat-line-scatter.png) | ![点线图](screenshots/flat-bar.png) |
|---|---|---|

#### fresh

| ![散点图](screenshots/fresh-scatter.png) | ![点线图](screenshots/fresh-line-scatter.png) | ![点线图](screenshots/fresh-bar.png) |
|---|---|---|

#### grape

| ![散点图](screenshots/grape-scatter.png) | ![点线图](screenshots/grape-line-scatter.png) | ![点线图](screenshots/grape-bar.png) |
|---|---|---|

#### grass

| ![散点图](screenshots/grass-scatter.png) | ![点线图](screenshots/grass-line-scatter.png) | ![点线图](screenshots/grass-bar.png) |
|---|---|---|

#### greyscale

| ![散点图](screenshots/greyscale-scatter.png) | ![点线图](screenshots/greyscale-line-scatter.png) | ![点线图](screenshots/greyscale-bar.png) |
|---|---|---|

#### light

| ![散点图](screenshots/light-scatter.png) | ![点线图](screenshots/light-line-scatter.png) | ![点线图](screenshots/light-bar.png) |
|---|---|---|

#### lilac

| ![散点图](screenshots/lilac-scatter.png) | ![点线图](screenshots/lilac-line-scatter.png) | ![点线图](screenshots/lilac-bar.png) |
|---|---|---|

#### pale

| ![散点图](screenshots/pale-scatter.png) | ![点线图](screenshots/pale-line-scatter.png) | ![点线图](screenshots/pale-bar.png) |
|---|---|---|

#### pink

| ![散点图](screenshots/pink-scatter.png) | ![点线图](screenshots/pink-line-scatter.png) | ![点线图](screenshots/pink-bar.png) |
|---|---|---|

#### sea

| ![散点图](screenshots/sea-scatter.png) | ![点线图](screenshots/sea-line-scatter.png) | ![点线图](screenshots/sea-bar.png) |
|---|---|---|

#### sky

| ![散点图](screenshots/sky-scatter.png) | ![点线图](screenshots/sky-line-scatter.png) | ![点线图](screenshots/sky-bar.png) |
|---|---|---|

#### solarized dark

| ![散点图](screenshots/solarized_dark-scatter.png) | ![点线图](screenshots/solarized_dark-line-scatter.png) | ![点线图](screenshots/solarized_dark-bar.png) |
|---|---|---|

#### solarized

| ![散点图](screenshots/solarized-scatter.png) | ![点线图](screenshots/solarized-line-scatter.png) | ![点线图](screenshots/solarized-bar.png) |
|---|---|---|

#### dracula

| ![散点图](screenshots/dracula-scatter.png) | ![点线图](screenshots/dracula-line-scatter.png) | ![点线图](screenshots/dracula-bar.png) |
|---|---|---|

### ScreenShots

线形图演示：

![线形图](screenshots/line_plot.gif)

柱状图演示：

![柱状图](screenshots/bar_plot.gif)

颜色映射图演示：

![颜色映射图](screenshots/color_map.gif)

### 依赖库
- [blitz](https://github.com/blitzpp/blitz/) - inline，无须单独编译
- [exprtkX](https://gitee.com/koala999/exprtk-x/)
- [KDDockWidgets](https://github.com/KDAB/KDDockWidgets/)
- [kfr](https://www.kfr.dev/) - inline，无须单独编译
- [libsndfile](http://libsndfile.github.io/libsndfile/)
- [praat](https://www.fon.hum.uva.nl/praat/) - inline，无须单独编译
- [QCustomPlot](https://www.qcustomplot.com/)
- [QtnProperty](https://github.com/koala999cn/QtnProperty/)
- [readerwriterqueue](https://github.com/cameron314/readerwriterqueue/) - inline，无须单独编译
- [rtaudio](http://www.music.mcgill.ca/~gary/rtaudio/)
- [smarc](http://audio-smarc.sourceforge.net/)
- [OpenXLSX](https://github.com/troldal/OpenXLSX)

目前提供以下预编译的依赖库(不包含OpenXLSX)
- [MSVC2019-x64-RELEASE](https://gitee.com/koala999/data-vis/attach_files/1156226/download)
- [MSVC2019-x64-DEBUG](https://gitee.com/koala999/data-vis/attach_files/1156225/download)