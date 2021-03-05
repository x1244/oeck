#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/Selection>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventAdapter>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/CommandManager>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <iostream>
using namespace std;
int main()
{
	float scale = 1.0;
	osgViewer::Viewer viewer;
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("../data/cow.osg");
	osg::ref_ptr<osgManipulator::Selection> selection = new osgManipulator::Selection;
	selection->addChild(loadedModel);

	//盒式拖拽器
	osgManipulator::TabBoxDragger* tad = new osgManipulator::TabBoxDragger();
	tad->setupDefaultGeometry();//使用默认的图形，即一个立方体
	tad->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(loadedModel->getBound().center()));
	tad->addTransformUpdating(selection);//Dragger要改变的对象。
	tad->setHandleEvents(true);

	//旋转拖拽器
	osgManipulator::TrackballDragger* dragger = new osgManipulator::TrackballDragger();
	dragger->setupDefaultGeometry();
	dragger->setMatrix(osg::Matrix::scale(scale*0.5, scale*0.5, scale*0.5)*osg::Matrix::translate(loadedModel->getBound().center()));
	dragger->addTransformUpdating(selection);
	dragger->setHandleEvents(true);

	//三维平移拖拽器
	osgManipulator::TranslateAxisDragger * Move3d = new osgManipulator::TranslateAxisDragger();
	Move3d->setupDefaultGeometry();
	Move3d->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(loadedModel->getBound().center()));
	Move3d->addTransformUpdating(selection);
	Move3d->setHandleEvents(true);

	osgManipulator::RotateCylinderDragger *rote = new osgManipulator::RotateCylinderDragger();
	rote->setupDefaultGeometry();
	rote->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(loadedModel->getBound().center()));
	rote->addTransformUpdating(selection);
	rote->setHandleEvents(true);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(selection);
	root->addChild(dragger);
	root->addChild(tad);
	root->addChild(Move3d);
	/*root->addChild(rote);*/

	viewer.setSceneData(root.get());
	return viewer.run();

}
