#include <osg/ref_ptr>
#include <osg/Image>
#include <osg/LineWidth>
#include <osg/Program>
#include <osgText/Text>
#include <osgViewer/Viewer>
#include <osgEarth/Controls>
#include <osgViewer/ViewerEventHandlers>

#include <osgEarth/Notify>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ElevationQuery>
#include <osgEarth/ExampleResources>
#include <osgEarth/FeatureNode>
#include <osgEarth/GeometryFactory>
#include <osgEarth/LogarithmicDepthBuffer>
#include <osgEarth/MapNode>
#include <osgEarth/PlaceNode>
#include <osgEarth/LabelNode>
#include <osgEarth/ModelNode>
#include <osgEarth/ShaderGenerator>
#include <osgEarth/Registry>
#include <osgEarth/Terrain>
#include <osgEarth/ThreadingUtils>
#include <osgDB/ReadFile>
#include <iostream>

#include <osgEarth/Metrics>
#include <iostream>

using namespace osg;
using namespace osgEarth;
using namespace osgEarth::Util;
using namespace std;
int main(int argc, char* argv[])
{
    osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
    osg::ref_ptr<osgEarth::Util::EarthManipulator> earthManipulator 
        = new osgEarth::Util::EarthManipulator;
    viewer->setCameraManipulator(earthManipulator);
    osg::ref_ptr<osg::Group> root = new osg::Group;
    osg::ref_ptr<osg::Node> earth = osgDB::readNodeFile("earthfile/simple.earth");

    root->addChild(earth);


    MapNode* mapNode = MapNode::findMapNode(earth);

    const SpatialReference* srs = mapNode->getMapSRS()->getGeographicSRS();

    root->addChild(earth);
    viewer->setSceneData(root.get());
    viewer->realize();
    osg::ref_ptr<osg::Group> planeGroup = new osg::Group;
    osg::ref_ptr<GeoTransform> gt = new GeoTransform;
    osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg");
    model->getOrCreateStateSet()->setMode(GL_LIGHTING
        , osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
    model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
    gt->addChild(mt);
    mt->addChild(model);
    planeGroup->addChild(gt);
    root->addChild(planeGroup);
    mt->setMatrix(osg::Matrix::scale(osg::Vec3(1000, 1000, 1000)));
    gt->setPosition(GeoPoint(srs, osg::Vec3(121.0, 38.0, 10000.), ALTMODE_ABSOLUTE));
    viewer->run();
    return 0;
}
#ifdef XXX
    osg::ref_ptr<osg::Node> plane = osgDB::readNodeFile("cessna.osg");
    plane->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribue::ON);
    osg::Matrix matrix;
    osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
    mt->addChild(plane);
    osg::ref_ptr<osg::MatrixTransform> mtfly = new osg::MatrixTransform;
    matrix.preMult(osg::Matrix::scale(2, 2, 2)
        *osg::Matrix::rotate(osg::inDegree(90.), osg::Vec3(0,0, 1)));
    mtfly->setMatrix(matrix);
    mtfly->addChild(mt);

    osg::ref_ptr<osg::Vec4Array> vatemp = new osg::Vec4Array;
    vatemp->push_back(osg::Vec4(116.0, 39.0, 60.0, 710));
    vatemp->push_back(osg::Vec4(116.0, 39.07, 50.0, 61.0));
    osg::AnimationPath* ap = new osg::AnimationPath;
#endif