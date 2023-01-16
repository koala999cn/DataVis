## provider
1. 支持hdf5, xlsx数据源
2. binary data loader

## plot
1. title等设置
2. 各种类型的axis
3. 分离axis
4. x轴与y轴的交换 --> ok
5. plot3d的legend布局 --> ok
6. colorbar --> ok
7. axis的反向设置 --> ok
8. 中文字体显示 --> ok（暂时使用imgui的中文字体支持 ）
9. opengl加速 --> ok
10. 导出图片 --> ok
11. 导出矢量图
12. auto-fold layout
13. subplot
14. graph的line-style --> ok
15. bar2d的opengl加速 --> ok
16.单独把heapmap成类（有text属性项）--> ok
17.统筹考虑plottable的颜色问题（flat vs smooth vs gradiant）--> ok
18. stacked & grouped bar
19. 色彩映射模式下的bar3d
20. 可配置色彩映射的数据维度 --> ok

## theme
1. 更多theme，SCI等
2. 进一步规范化specification
3. 完善plot2d和plot3d的layout主题

## pipeline
1. 时间同步的问题。比如信号发生器后面接入audiopalyer，产生的数据会在audiopalyer缓存队列中越积越多
2. 管线运行时，node属性的动态设置与同步
3. 给数据加时间戳stamp

## FIXME
1. 使用layout系统后，plot的axis留白出现问题（tick-label超出coord的innerRect区域）
2. 三维坐标系下，虚线的绘制（ImGui）在旋转到特定方向时会出现错位 --> 使用opengl绘制后，该问题没再出现，估计由于计算误差引起
3. 加载大的text数据文件很慢
4. 坐标轴刻度旋转时出现突变
5. 3d透视投影模式下，坐标轴的刻度线和文字全部消失 --> ok（一是矢量投影算法问题，二是逆变换未作归一化）
6. plot3d在交换坐标轴后，或者在透视投影模式下，鼠标操控坐标系姿态出现问题 --> ok
7. plottable不可见/可见设置时的legend同步 --> ok
8. colormap的border属性项有效性 --> ok
9. 当坐标系的x或y轴尺寸很小时，鼠标移动plot3d有问题 --> ok
10. colorbar与颜色渲染模式的同步 --> ok
11. 多通道数据绘图二次启动crack --> ok
12. 窗口缩放到很小时，layout未被正确设置，导致坐标轴被剪切，plt和legend绘制到外面 --> 设置legend的clipRect后，该问题没再出现
13. plot2d在启用深度测试的时候有问题，而且plot各元素的layer顺序有问题