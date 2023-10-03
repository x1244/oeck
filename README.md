# oeck

osgearth-ck

基于osgearth做一些3D展示

做这个项目的起因是需要做一些离线地图的展示，我的初步想法是基于osgEarth来做。
在离线地图上显示一些汽车、飞机等模型，结合运动轨迹、姿态等显示出来。

任何人如果兴趣的话都可以获取项目内容，至于怎么使用它完全由您，但要遵守你那里的法律规定。

# build

## 预编译库

我将vs2022的预编译库放在了网盘上，链接：https://pan.baidu.com/s/1vtnv-cf76TbS9N6cc-0avw 
提取码：abab。

## vs2022

```
> git clone https://github.com/x1244/oeck.git
> cd oeck
oeck> xmake
oeck> viewer simple.earth
```

* 20221227更新

在vs2022下重新编译了大部分依赖库。

以`GL3`模式编译了`OpenSceneGraph`。

更新了编译手册，预编译的库放在了网盘上。

只保留了地球加载的演示，其他示例因为`osgEarth` 更新还没有适配，感觉这次版本变动很大。

# Demo

一些小的示例放在了demo文件夹下，每个示例在一个子文件夹下。

* pick，选取标注和Feature。不仅可选取`.earth`配置的标注，还可选取程序中添加的标注。