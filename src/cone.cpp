
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osgEarth/EarthManipulator>
#include <osg/MatrixTransform>
#include <osgEarth/ConvertTypeFilter>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgEarth/ExampleResources>


using namespace osgEarth;
using namespace osgEarth::Util;
int usage(const char* name)
{
    OE_NOTICE 
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}
// lonlat1:雷达波圆锥顶点
// lonlat2:轨迹点
void rotateCone(osg::MatrixTransform* mt, const osgEarth::SpatialReference* sr, osg::Vec3d lonlat1, osg::Vec3d lonlat2)
{
    // 雷达波模型所在位置
    osgEarth::GeoPoint geoPoint1(
        sr,
        lonlat1.x(),
        lonlat1.y(),
        lonlat1.z(),
        osgEarth::ALTMODE_ABSOLUTE);
    osg::Matrixd matrix1;
    // 获取雷达波模型从原点变换到lonlat1位置的变换矩阵
    geoPoint1.createLocalToWorld(matrix1);
    // 经纬度高程到xyz的变换
    osg::Vec3d world1, world2;
    // geoPoint1.toWorld(world1);//等同于    sr->transformToWorld(lonlat1,world1);
    sr->transformToWorld(lonlat2, world2);
    // 计算轨迹点在雷达波模型坐标系下的位置
    osg::Vec3 point2InRadarCoordinateSystem = world2*osg::Matrix::inverse(matrix1);
    // 在雷达波模型坐标系下，对Z轴进行旋转，与连接原点指向轨迹点方向的矢量重合，计算出此旋转矩阵
    osg::Matrixd rotMat = osg::Matrixd::rotate(osg::Z_AXIS, point2InRadarCoordinateSystem-osg::Vec3(0,0,0));
    // 将计算出的旋转矩阵赋给雷达波模型所在的mt
    mt->setMatrix(rotMat);
}
int main(int argc, char** argv)
{
	osg::ArgumentParser arguments(&argc,argv);

    osgViewer::Viewer viewer;
	
	osg::Node* node = MapNodeHelper().load(arguments, &viewer);
    MapNode* mapNode = MapNode::findMapNode(node);
    if ( !mapNode )
        return usage(argv[0]);

    //opt.url() = "cow.osg.1000,1000,1000.scale";
    osg::Geode* geode = new osg::Geode;
    osg::ShapeDrawable* cone = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(), 10000, 50000));
    //osg::ShapeDrawable* cone = new osg::ShapeDrawable(new osg::Box(osg::Vec3(), 50000));
    geode->addDrawable(cone);
    osg::MatrixTransform* mtCone = new osg::MatrixTransform;
    mtCone->addChild(geode);

    rotateCone(mtCone, mapNode->getMapSRS()->getGeographicSRS(), osg::Vec3(118, 40, 100), osg::Vec3(120, 40, 100));

    osg::Group* root = new osg::Group();
    root->addChild(mapNode);
    viewer.setSceneData(root);
    viewer.setCameraManipulator(new osgEarth::Util::EarthManipulator());

    // Process cmdline args
    //MapNodeHelper().parse(mapNode, arguments, &viewer, root, new LabelControl("Features Demo"));

    //视点定位模型所在位置
    osgEarth::Viewpoint vp("", 118, 40, 1000.0, -2.50, -90.0, 1.5e6);
    (dynamic_cast<osgEarth::Util::EarthManipulator*>(viewer.getCameraManipulator()))->setViewpoint(vp);

    // add some stock OSG handlers:
    viewer.addEventHandler(new osgViewer::StatsHandler());
    viewer.addEventHandler(new osgViewer::WindowSizeHandler());
    viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    viewer.setUpViewInWindow(100, 500, 1024, 768);
    return viewer.run();
}