# 国境线和省界线

国境线/省界线的添加有两种方式：

- 基于model
- 基于OGRFeatures和FeatureModel

无论是哪种方式添加，注意`.shp`文件是一套，包括`.shp`、`.dbf`、`.prj`、`.shx`。

## 基于model

见osgearth参考文档 http://docs.osgearth.org/en/latest/references/drivers/model/feature_geom.html

```
	<model driver="feature_geom">
    <features driver="ogr">
        <url>../data/world.shp</url>
    </features>
    <styles>
        <style type="text/css">
            default {
                stroke:       #ffff00;
                stroke-width: 2;
            }
        </style>
    </styles>
    <fading duration="1.0"/>
    </model>
```

## 基于OGRFeatures和FeatureModel

参考`tests/feature_geom.earth`

```
    <OGRFeatures name="world">
        <url>../data/world.shp</url>
    </OGRFeatures>
    
    <FeatureModel name="world boundary" features="world">
        <styles>
            <style type="text/css">
                states {
                   stroke:          #ffff00; 
                   stroke-width:    2px;
                   render-depth-offset: true;
                }                    
            </style>
        </styles>        
    </FeatureModel>
```

# HUD

`HUD`的实现有两种方式，基于`osg`的前景`camera`模式和基于`osgearth`控件画布方式。基于`camera`的方式有更多的实现和更多的参考，基于`ControlCanvas`的实现相对适用于静态HUD。

在使用`camera`实现时，在地球由`MapNodeHelper::load`加载时，如果未指定`--nologdepth`，则启用对数深度缓冲区，`camera`无法正确渲染，而`ControlCanvas`不受对数深度缓冲区影响。这个应该可以设置`camear`的z来实现，待尝试。

`ControlCanvas`对于动态HUD的渲染，可以通过`ImageControl`来实现，需要使用RTT技术，先渲染到纹理，再设置到`ImageControl`，正在进行尝试。

对HUD的camera设置一个空的着色器程序，以取代osgearth默认的深度缓冲区筛选，可解决`MapNodeHelper::load`加载，默认使用`logdepth`的问题。

# 经纬度高程

`osgearth_measure`给出了一个很好的经纬度高程获取方案，主要由以上代码完成

```
osg::Vec3d world;
if ( _mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world) ){
    GeoPoint map;
    map.fromWorld( _mapNode->getMapSRS(), world );
}
```

经过测试，`getHeight`方法也可获得指定点高程

```
double h, e;
_mapNode->getTerrain()->getHeight(_mapNode->getMapSRS(), map.x(), map.y(), &h, &e);
```

但是另一处资料中的`ElevationQuery`并不能获取高程，代码如下，原因未知。

```
osgEarth::ElevationQuery query(_mapNode->getMap());
GeoPoint p(_mapNode->getMapSRS(), map.x(), map.y());
double r2;
float qh = query.getElevation(p, 0., &r2);
```

# 片元着色器

片元着色器输出的是最后的像素渲染结果，所以只需要一个输出`out vec4 color`即可，至于到缓冲区中的输出，还有待跟进。

# OpenGL

OpenGL 3.1以上的API中已经移除了 `glBegin / glEnd`