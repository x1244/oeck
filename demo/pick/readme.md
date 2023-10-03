# 问题

解决地球上标注的选取。osgEarth源码提供了一个选取的示例`osgearth_pick`，这也是这项工作的基础。但是那个选取只能选择由`.earth`文件加载的标注，不能选取程序中添加的标注。

# 标注添加

标注的添加有两种方式，一种是将标注加到组中，再添加到`MapNode`节点下，这是官方示例`osgearth_annotation`提供的方式。另一种方式我认为是将标注添加到`AnnotationLayer`层下，再将层添加到`Map`下，这样的组织方式更合适一些。

# 原因分析

osgEarth中的标注选取是通过`osgEarth::Util::ObjectIDPicker`来实现的，而程序添加的标注没能识别出来是因为ObjectID的问题。通过`.earth`添加的标注有`ObjectIndex`，所以能被识别出来，所以要在程序中添加的标注也加上ID。

# 具体实现

找到原因了，实现代码很简单

```cpp
void createCustomAnno(osgEarth::MapNode* root)
{
    using namespace osgEarth;
    osgEarth::Map* themap = root->getMap();
    osgEarth::AnnotationLayer* layer = new osgEarth::AnnotationLayer;
    themap->addLayer(layer);
    Style pm;
    pm.getOrCreate<TextSymbol>()->encoding() = TextSymbol::ENCODING_UTF8;
    pm.getOrCreate<TextSymbol>()->font() = "C:/Windows/Fonts/simhei.ttf";

    auto srs = themap->getSRS();
    osg::ref_ptr<osg::Image> img = osgDB::readRefImageFile("../data/placemark32.png");
    IC(img);
    auto anno = new PlaceNode(GeoPoint(srs, -74.00, 40.71), "标识符", pm, img);
    anno->setName("测试标识");
    layer->addChild(anno);

    anno->setMapNode(root);
    Registry::objectIndex()->tagNode(anno, anno);
}
```

其中`tagNode`就是给标注赋予ID，注意还要将`anno`与地球根节点`MapNode`关联起来。

这其中还解决了典型的中文问题，在`windows`下，`vs2022`编译环境，源码以`utf-8`编码，中文可直接识别不用转换，但在应用样式指定编码为`UTF8`，并且给出正确的字体路径。

```cpp
Style pm;
pm.getOrCreate<TextSymbol>()->encoding() = TextSymbol::ENCODING_UTF8;
pm.getOrCreate<TextSymbol>()->font() = "C:/Windows/Fonts/simhei.ttf";
```




