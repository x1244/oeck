#include <osgViewer/Viewer>
#include <osg/Notify>
#include <osgDB/ReadFile>
#include <osgEarth/Notify>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/MapNode>
#include <osgEarth/Threading>
#include <iostream>
#include <string>
#include <osgEarth/Metrics>
#include <osg/DisplaySettings>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#define LC "[viewer] "

using namespace std;
using namespace osgEarth;
using namespace osgEarth::Util;
class logHandler : public osg::NotifyHandler{
public:
void notify(osg::NotifySeverity severity, const char* msg) override{
    string str(msg);
    std::size_t end = str.find_last_not_of("\r\n");
    if(end != std::string::npos){
        str.erase(end + 1);
    }else{}
    if(str.empty()){
        return;
    }
    switch(severity){
        case osg::FATAL:
            spdlog::critical("{{osg}} {}", str);
            break;
        case osg::WARN:
            spdlog::warn("{{osg}} {}", str);
            break;
        case osg::NOTICE:
            spdlog::info("{{osg}} {}", str);
            break;
        case osg::INFO:
            spdlog::info("{{osg}} {}", str);
            break;
        case osg::DEBUG_INFO:
            spdlog::debug("{{osg}} {}", str);
            break;
        case osg::DEBUG_FP:
            spdlog::debug("{{osg}} {}", str);
            break;
    }
}
};
osg::Camera* createHud(std::string strImg);
int usage(const char* name)
{
    OE_NOTICE
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}
void logger()
{
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");
	auto logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", false);
	spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, logger})));
#ifdef XXLOG
	spdlog::info("spd 测试");
	spdlog::info("spd log");
	spdlog::warn("spd warning");
	spdlog::error("spd cirtical");
#endif
}
int main(int argc, char** argv)
{
	//OSG_NOTICE <<"测试" <<endl;
    osg::setNotifyHandler(new logHandler);
	logger();
    osgEarth::initialize();
    osg::ArgumentParser arguments(&argc,argv);
    if ( arguments.read("--help") ) return usage(argv[0]);
	//osg::setNotifyLevel(osg::NotifySeverity::INFO);
	//osg::DisplaySettings::instance()->setShaderPipeline(true);
    osgViewer::Viewer viewer(arguments);
    viewer.getDatabasePager()->setUnrefImageDataAfterApplyPolicy( true, false );
    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");
    viewer.setCameraManipulator( new EarthManipulator(arguments) );
    viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    osg::Node* node = MapNodeHelper().load(arguments, &viewer);
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild(node);
    osg::Camera* hud = createHud("../data/5.png");
    root->addChild(hud);
    viewer.setSceneData( root );
    return Metrics::run(viewer);
}

osg::Camera* createHud(std::string strImg)
{
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = 1920;
    traits->height = 200;
    traits->windowDecoration = false;
    traits->sharedContext = 0;
	
    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
    osg::ref_ptr<osg::Camera> camera1 = new osg::Camera;
    camera1->setGraphicsContext(gc);
    camera1->setAllowEventFocus(false);
    camera1->setProjectionMatrixAsOrtho2D(0, 1920, 0, 200);
    camera1->setViewport(0, 0, 1920, 200);

    camera1->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera1->setRenderOrder(osg::Camera::POST_RENDER);
//    camera1->setRenderOrder(osg::Camera::NESTED_RENDER);
    camera1->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    camera1->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    camera1->setClearColor(osg::Vec4(0.5, 0.3, 0.2, 0.1));
    camera1->setViewMatrix(osg::Matrix::identity());

    osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry1 = new osg::Geometry;
    //vertex
    const double z = -1.;
    osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
    vertex->push_back(osg::Vec3(0.0,0.0,z));
    vertex->push_back(osg::Vec3(1920.0, 0.0, z));
    vertex->push_back(osg::Vec3(1920.0, 200.0, z));
    vertex->push_back(osg::Vec3(0.0, 200.0, z));
    geometry1->setVertexArray(vertex);
    //normal
    osg::ref_ptr<osg::Vec3Array> norml = new osg::Vec3Array;
    norml->push_back(osg::Vec3(0.0, 0.0, 1.0));
    geometry1->setNormalArray(norml);
    geometry1->setNormalBinding(osg::Geometry::BIND_OVERALL);
    //texture
    osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array;
    coord->push_back(osg::Vec2(0.0, 0.0));
    coord->push_back(osg::Vec2(1.0, 0.0));
    coord->push_back(osg::Vec2(1.0, 1.0));
    coord->push_back(osg::Vec2(0.0, 1.0));
    geometry1->setTexCoordArray(0, coord);
    geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    osg::ref_ptr<osg::Image> img1 = osgDB::readImageFile(strImg);

    osg::ref_ptr<osg::Texture2D> texture2d = new osg::Texture2D;
    texture2d->setImage(0, img1);
    geometry1->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture2d,osg::StateAttribute::ON);

    geode1->addDrawable(geometry1);
	osg::ref_ptr<osg::StateSet> ss = geode1->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    camera1->addChild(geode1);
    return camera1.release();
}
