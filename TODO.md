## overall
- [ ] 增加图标
   https://github.com/FortAwesome/Font-Awesome & https://github.com/mnesarco/bawr
   https://www.microsoft.com/en-us/download/details.aspx?id=35825
   https://github.com/godotengine/godot/tree/master/editor/icons
- [ ] 本地化（参考godot）
- [ ] Undo/Redo（参考 https://github.com/ocornut/imgui/issues/1875 ）


## provider
- [ ] 支持hdf5, xlsx数据源
- [ ] binary data loader
- [x] 多值函数支持(multifunction)
- [x] 多通道连续数据
- [x] 缓存function的range计算
- [ ] 整合expression和function，提供parametric标记，并新增实现piecewise expression
- [ ] 文本文件中日期的解析（参考http://www.partow.net/programming/datetime/index.html ）
- [x] 可选择通道
- [ ] 解析数据列或通道的label


## operator
- [ ] 测试spectrum的options的sampleRate一致性
- [x] sampler的采样率动态更改后，输出没有更新 -> 主要是audioplay没有对输入重采样，也没有变换设备频率
- [ ] 完善kde（参考https://github.com/timnugent/kernel-density）
- [ ] 检测kde结果的正确性（https://pythonawesome.com/plotting-beautiful-ridgeline-plots-in-python/ ）

## plot
- [ ] title等设置
- [ ] 各种类型的axis
- [ ] 分离axis
- [ ] 文字渲染系统（参考https://github.com/mosra/magnum/tree/master/src/Magnum/Text ）
- [x] x轴与y轴的交换 --> ok
- [x] plot3d的legend布局 --> ok
- [x] colorbar --> ok
- [x] axis的反向设置 --> ok
- [x] 中文字体显示 --> ok（暂时使用imgui的中文字体支持 ）
- [x] opengl加速 --> ok
- [x] 导出图片 --> ok
- [ ] 导出矢量图
- [ ] auto-fold layout
- [ ] subplot
- [x] graph的line-style --> ok
- [x] bar2d的opengl加速 --> ok
- [x] 单独把heapmap成类（有text属性项）--> ok
- [x] 统筹考虑plottable的颜色问题（flat vs smooth vs gradiant）--> ok
- [x] stacked & grouped bar -->ok
- [x] 色彩映射模式下的bar3d
- [x] 可配置色彩映射的数据维度 --> ok
- [x] legend支持单通道的多主色plottable --> ok
- [x] sactter图的美化问题。带轮廓线后，各scatter轮廓相互干扰，非常难看，启动深度测试又因为z-fighting也不好看。（#I6B5ES）
- [x] 色带的编辑
- [ ] 光照模型
- [x] 辅色控制
- [ ] 坐标平面水平/垂直坐标轴对的ticks数不同时，如何绘制grid
- [ ] 给gradient控件添加1个选择pallete的按钮
- [ ] 优化海量数据的降采样
- [x] 优化vbo的构建，防止重复构建同一时间戳数据的vbo
- [x] plt隐藏/可见切换时对vbo复用的影响 -> 为兼顾动态数据，隐藏时不绘制（不复用）
- [x] plottable侧对复用vbo的通用框架
- [x] ridgeplot
- [x] 密度图（参考ggplot2的geom_density）-> 实现operator KDE
- [ ] 色带编辑器美化（参考https://github.com/CoolLibs/imgui_gradient & https://github.com/effekseer/ImGradientHDR ）
- [x] plot的autorange由KvRdPlot负责，根据provider调整，而非data
- [x] paddingStacked的语义一致性
- [x] text支持vbo重用
- [x] 完善clip的rect修订，legend的border的有的边会被非正常剪切 --> 绘制item时才设置clip
- [x] 完善bars2d的padding设置（是否使用像素值？）
- [ ] range太大时axis不能正常定位tick和label
- [x] 所有plot1d都按照兼容stack和ridge模式重构
- [x] 启动深度测试时，坐标轴的baseline与gridline深度冲突
- [ ] 三维面积图（参考https://blog.csdn.net/Peter_Zhang_Jie/article/details/49995213）
- [ ] fitColorMappingRange考虑arrange模式
- [ ] 测试多通道高维数据的arrange模式
- [x] 实现plottable2d的arrange模式
- [ ] heatmap的维度映射
- [x] arrange模式增加zshift实现
- [ ] 面积图支持below & above控制项（参考https://gnuplot.sourceforge.net/demo/fillbetween.html ）
- [x] 散点图支持渐变色
- [x] 散点图兼容气泡图
- [x] 散点图的告示牌模式
- [x] text复用情况下的状态同步：①变换矩阵设置，②保持恒定尺寸
- [ ] 散点图启用size varying之后的性能问题（以wav.txt为例）
- [ ] plot3d的colorbar布局问题
- [ ] andres曲线的autoRange不正确
- [ ]boxplot的初始尺寸太小

## theme
- [ ] 更多theme，SCI、ggthemes等
- [ ] 进一步规范化specification
- [ ] 完善plot2d和plot3d的layout主题
- [x] 各要素的可见性设置不改变linesyle和alpha等属性 

## pipeline
- [x] 时间同步的问题。比如信号发生器后面接入audiopalyer，产生的数据会在audiopalyer缓存队列中越积越多 -> 静态数据变化时清空队列
- [x] 管线运行时，node属性的动态设置与同步
- [x] 给数据加时间戳stamp
- [x] 考虑管线未启动时的数据时间戳问题，如何体现数据更新？--> 是否动态operate和render
- [x] 追踪和管理operator配置的变化状态
- [ ] 实现消息框（参考https://github.com/leiradel/ImGuiAl）
- [ ] node-editor的连接错误消息提示（考虑渐消隐的tooltip）
- [x] pipeline非运行状态的数据同步，节点连接管理（若数据参数更改后变得不可连接，强制断开）
- [ ] 更全面地测试数据维度和通道数等发生变化的情况

## FIXME
- [ ] 启用多重采样之后，程序启动短暂白屏 --> 貌似和多重采样无关
- [x] graph和scatter对arrange模式的支持
- [x] box和andrews曲线的鲁棒性（渲染samp1d数据导致致命错误）--> setData中没有及时output
- [x] 散点图在plot3d下干扰imgui绘制 --> ok （未恢复多实例渲染状态，即重置glVertexAttribDivisor）
- [x] 使用layout系统后，plot的axis留白出现问题（tick-label超出coord的innerRect区域）
- [x] colorbar的axis留白同步
- [x] 三维坐标系下，虚线的绘制（ImGui）在旋转到特定方向时会出现错位 --> 使用opengl绘制后，该问题没再出现，估计由于计算误差引起
- [x] 加载大的text数据文件很慢
- [ ] 坐标轴刻度旋转时出现突变
- [x] 3d透视投影模式下，坐标轴的刻度线和文字全部消失 --> ok（一是矢量投影算法问题，二是逆变换未作归一化）
- [x] plot3d在交换坐标轴后，或者在透视投影模式下，鼠标操控坐标系姿态出现问题 --> ok
- [x] plottable不可见/可见设置时的legend同步 --> ok
- [x] colormap的border属性项有效性 --> ok
- [x] 当坐标系的x或y轴尺寸很小时，鼠标移动plot3d有问题 --> ok
- [x] colorbar与颜色渲染模式的同步 --> ok
- [x] 多通道数据绘图二次启动crack --> ok
- [x] 窗口缩放到很小时，layout未被正确设置，导致坐标轴被剪切，plt和legend绘制到外面 --> 6b73a81
- [x] plot2d在启用深度测试的时候有问题 --> d209879f
- [ ] legned的border被plottable遮挡 --> 主要因为部分元素仍使用ImGui绘制
- [x] legend的warp把rowMajor搞反了 --> ok
- [x] legend在外侧布局的时候仍然没有正常裁剪 --> 布局系统bug（6b73a81）
- [x] 坐标轴基线与plt区域完美贴合的问题（line-filled区域在下方多1个像素，右方少1个像素）
- [x] 有时plane背景色和grid线无法正常显示，需要通过重新应用theme来出发 --> ok（缺少plane的可见性设置）
- [x] ticker数的控制存在致命bug（上下或左右或前后未同步）--> 暂时取消assert检测
- [x] 优化flat着色模式下的line-filled
- [x] 坐标轴只显示title时的定位问题
- [x] spectrum操作符的坐标平移问题
- [x] audioplayer渲染器在启动后再删除的情况下（哪怕启动失败），有个内存非法写入bug，后来不启动也会出现该问题 --> 重新编译后该问题消失
- [x] andrews曲线生成错误 --> 系数访问越界
- [x] andrews曲线的样子还是和网上的对不上 --> mtcars数据的第2列代表类别，不应参与andrews曲线计算
- [x] plt的aabb计算更新只考虑了数据和颜色，未考虑绘图切换stacked样式等情况