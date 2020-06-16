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

