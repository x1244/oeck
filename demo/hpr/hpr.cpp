/**\file
 *
 * \brief 设备方位俯仰跟踪演示
 * \author x1244 <x11244@126.com>
 *
 * 基于osgSim::DOFTransform来更新设备的方位俯仰
 */
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgSim/DOFTransform>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgSim/DOFTransform>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/BlendFunc>
#include <icecream.hpp>
#include "ttp.h"
#include "ttp_update.h"
using namespace std;
/**\brief 创建设备
 *
 * \return osg::Node* - 设备节点
 */
osg::Node* createTtp();
/**\brief 创建单一DOFTransform节点
 *
 * 创建节点测试
 *
 * \return osg::Node* - 节点
 */
osg::Node* createHpr();
int main(int argc, char* argv[])
{
    osg::ref_ptr<osg::Group> root = new osg::Group;
    auto ss = root->getOrCreateStateSet();
    osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc;
    bf->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ss->setAttributeAndModes(bf);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    root->addChild(osgDB::readNodeFile("axes.osgt"));
    root->addChild(createTtp());
//    root->addChild(createHpr());
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    viewer.run();
    return 0;
}
osg::Node* createTtp()
{
    using namespace osg;
#if 0
    //auto doft = new osgSim::DOFTransform;
    auto doft = new osg::MatrixTransform;
//    auto doft = new osg::Group;
    auto base = new osg::ShapeDrawable;
    base->setShape(new osg::Box(osg::Vec3(0., 0., 0.), 10., 4., 0.5));
    doft->addChild(base);
    IC(doft->getNumChildren());
#endif
    Ttp* ttp = new Ttp;
    ttp->addUpdateCallback(new TtpUpdate);
    auto base = new osg::ShapeDrawable;
    base->setShape(new osg::Box(osg::Vec3(0., 0., 0.), 10., 4., 0.5));
    base->setColor(osg::Vec4(.75, .55, .1, .2));
    auto apart = new osg::ShapeDrawable;
    apart->setShape(new osg::Box(osg::Vec3(0., 0., 0.), 1., 1., 5.));
    apart->setColor(osg::Vec4(.2, .66, .15, .2));
    auto epart = new osg::ShapeDrawable;
    epart->setShape(new osg::Box(osg::Vec3(-1.8, 0., 0.), 4., 2., 1.));
    epart->setColor(osg::Vec4(.64, .2, .64, .3));
    ttp->buildBase(base);
    ttp->buildApart(apart, osg::Vec3(4.8, 0., 2.3));
    ttp->buildEpart(epart, osg::Vec3(0., 0., 2.2));
    ttp->updateAe(-30., 30.);
//    ttp->buildTtp(base, apart, epart);
    auto node = new PositionAttitudeTransform;
    node->addChild(ttp);
    node->setPosition(Vec3(4.2, 0., .3));
    return node;
}
osg::Node* createHpr()
{
    using osgSim::DOFTransform;
    using namespace osg;
    DOFTransform* node = new DOFTransform;
    Geode* g = new Geode;
    auto base = new osg::ShapeDrawable;
    base->setShape(new osg::Box(osg::Vec3(0., 0., 0.5), 10., 4., 0.5));
    g->addDrawable(base);
    node->addChild(g);
//    node->addChild(osgDB::readNodeFile("cow.osg"));
//    osg::Vec3 sc = node->getCurrentScale();
//    IC(sc[0], sc[1], sc[2]);
    node->setCurrentScale(osg::Vec3(1., 1., 1.));
    node->setCurrentTranslate(osg::Vec3(5., 0., 0.));
    node->setCurrentHPR(osg::Vec3(3.14/6., 0., 0.));
    return node;
}