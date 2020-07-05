#include <osgViewer/Viewer>
#include <osgEarth/Notify>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/MapNode>
#include <osgEarth/PlaceNode>
#include <osgEarth/LabelNode>
#include <osgEarth/ModelNode>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Registry>
#include <osgEarth/ThreadingUtils>
#include <osgDB/ReadFile>
#include <iostream>

#include <osgEarth/Metrics>

#include <Windows.h>

namespace
{
	void adjustWindow(osgViewer::Viewer*viewer,
		const unsigned int x, const unsigned int y,
		const unsigned int w, const unsigned int h)
	{
		osg::ref_ptr<osg::GraphicsContext::Traits>trait = new osg::GraphicsContext::Traits;
		trait->x = x;
		trait->y = y;
		trait->width = w;
		trait->height = h;
		trait->doubleBuffer = true;
		trait->windowDecoration = true;
		trait->sharedContext = 0;

		osg::ref_ptr<osg::GraphicsContext>gc = osg::GraphicsContext::createGraphicsContext(trait);
		viewer->getCamera()->setGraphicsContext(gc);
		viewer->getCamera()->setViewport(new osg::Viewport(0, 0, trait->width, trait->height));
		GLenum buffer = trait->doubleBuffer ? GL_BACK : GL_FRONT;
		viewer->getCamera()->setDrawBuffer(buffer);
		viewer->getCamera()->setReadBuffer(buffer);
	}
}

int main(int argc, char* argv[])
{
	osg::ref_ptr<osgViewer::Viewer>viewer = new osgViewer::Viewer;
	//操作器
	osg::ref_ptr<osgEarth::Util::EarthManipulator> earthManipulator = new osgEarth::Util::EarthManipulator;
	viewer->setCameraManipulator(earthManipulator);
 	//根节点
	osg::ref_ptr<osg::Group>root = new osg::Group;

	//加载地球节点
	osg::Node* earthNode = osgDB::readNodeFile("simple.earth");
	root->addChild(earthNode);

	osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode(earthNode);
	if (!mapNode) return 0;			
	const osgEarth::SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
	
	//添加模型
	{
		osg::Node* model = osgDB::readNodeFile("../data/cow.osgt");
		//osg中光照只会对有法线的模型起作用，而模型经过缩放后法线是不会变得，
		//所以需要手动设置属性，让法线随着模型大小变化而变化。GL_NORMALIZE 或 GL_RESCALE_NORMAL
		model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

		osg::Matrix Lmatrix;
		geoSRS->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(40.0), osg::DegreesToRadians(116.0), 100000.0, Lmatrix);
		//放大一些，方便看到
		Lmatrix.preMult(osg::Matrix::scale(osg::Vec3(10000, 10000, 10000)));

		osg::MatrixTransform* mt = new osg::MatrixTransform;
		mt->setMatrix(Lmatrix);
		mt->addChild(model);
		root->addChild(mt);
	}
	viewer->setSceneData(root);

	//配置窗口
	adjustWindow(viewer, 100, 100, 720, 576);
	
	//视点定位北京地区
	earthManipulator->setViewpoint(osgEarth::Viewpoint("", 116, 40, 0.0, -2.50, -90.0, 1.5e6));
	
	viewer->run();
	return 0;
}
