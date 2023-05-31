/**\file
 *
 * \brief 风场可视化
 * \author x1244 <x11244@126.com>
 */
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/Metrics>
#include <osgEarth/Notify>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/SpatialReference>
#include <osgEarth/Threading>

#include <iostream>
#include <icecream.hpp>
#include "TempeLayer"

#define LC "[viewer] "

using namespace std;
using namespace osg;
using namespace osgEarth;
using namespace osgEarth::Util;

int main(int argc, char** argv)
{
    osgEarth::initialize();

    osg::ArgumentParser arguments(&argc,argv);
    auto* usage = arguments.getApplicationUsage();
    usage->setApplicationName(arguments.getApplicationName());
    usage->setDescription(arguments.getApplicationName() + " 数字地球温度场");
    usage->setCommandLineUsage(arguments.getApplicationName()
        +" [options]... earthfile");
    usage->addCommandLineOption("--image <temperature image>", "温度图像");

    if(arguments.errors()){
        arguments.writeErrorMessages(std::cout);
        return 1;
    }
    if(arguments.argc() < 2){
        arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return 1;
    }
	string img("z3.png");
    arguments.read("--image", img);



    // load the .earth file from the command line.
    osg::ref_ptr< osg::Node > earthNode = osgDB::readRefNodeFiles( arguments );
    if (!earthNode || !osgEarth::MapNode::findMapNode(earthNode)){
        cout <<"配置地球失败" <<endl;
        return 0;
    }
	osg::ref_ptr<osg::Image> src = osgDB::readImageFile(img);
	if(!src.valid()){
        cout <<"加载温度场图像" <<img <<"失败" <<endl;
	    return 0;
    }

    osgEarth::MapNode* earth = osgEarth::MapNode::findMapNode(earthNode);
    osgEarth::TemperatureLayer* temp = new osgEarth::TemperatureLayer;
    earth->getMap()->addLayer(temp);
    osg::ref_ptr<osg::Texture> tex = new osg::Texture2D(src);
    temp->applyTempSrc(tex);

    osgViewer::Viewer viewer(arguments);
    viewer.setReleaseContextAtEndOfFrameHint(false);
    viewer.getDatabasePager()->setUnrefImageDataAfterApplyPolicy( true, false );
    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");
    viewer.setCameraManipulator( new EarthManipulator(arguments) );
    viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
    // add some stock OSG handlers:
    viewer.addEventHandler(new osgViewer::StatsHandler());
    viewer.addEventHandler(new osgViewer::WindowSizeHandler());    
    viewer.addEventHandler(new osgViewer::LODScaleHandler());
    viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    viewer.addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

    ref_ptr<Group> root = new Group;
    root->addChild(earthNode);

    viewer.setSceneData(root);
    return Metrics::run(viewer);
}