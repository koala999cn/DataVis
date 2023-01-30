## provider
- [ ] 支持hdf5, xlsx数据源
- [ ] binary data loader
- [ ] 多值函数支持(multifunction)
- [ ] 多通道连续数据

## plot
- [ ] title等设置
- [ ] 各种类型的axis
- [ ] 分离axis
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
- [ ] 色彩映射模式下的bar3d
- [x] 可配置色彩映射的数据维度 --> ok
- [x] legend支持单通道的多主色plottable --> ok
- [x] sactter图的美化问题。带轮廓线后，各scatter轮廓相互干扰，非常难看，启动深度测试又因为z-fighting也不好看。（#I6B5ES）
- [x] 色带的编辑
- [ ] 光照模型
- [x] 辅色控制
- [ ] 坐标平面水平/垂直坐标轴对的ticks数不同时，如何绘制grid
- [ ] 给gradient控件添加1个选择pallete的按钮
- [ ] 优化海量数据的降采样

## theme
- [ ] 更多theme，SCI等
- [ ] 进一步规范化specification
- [ ] 完善plot2d和plot3d的layout主题
- [ ] 各要素的可见性设置不改变linesyle和alpha等属性 

## pipeline
- [ ] 时间同步的问题。比如信号发生器后面接入audiopalyer，产生的数据会在audiopalyer缓存队列中越积越多
- [ ] 管线运行时，node属性的动态设置与同步
- [ ] 给数据加时间戳stamp

## FIXME
- [ ] 使用layout系统后，plot的axis留白出现问题（tick-label超出coord的innerRect区域）
- [ ] 三维坐标系下，虚线的绘制（ImGui）在旋转到特定方向时会出现错位 --> 使用opengl绘制后，该问题没再出现，估计由于计算误差引起
- [ ] 加载大的text数据文件很慢
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
- [ ] 坐标轴只显示title时的定位问题
