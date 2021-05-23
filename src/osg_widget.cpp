#include "osg_widget.h"
#include <iostream>
#include <QMessageBox>
#include <QKeyEvent>
#include <osgQOpenGL/OSGRenderer>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osg/Types>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osgGA/Device>

#include <QApplication>
#include <QSurfaceFormat>
#include <osg/ArgumentParser>
using namespace std;
OsgWidget::OsgWidget(QWidget* parent)
  : osgQOpenGLWidget(parent)
{
}
OsgWidget::OsgWidget(osg::ArgumentParser* arguments, QWidget* parent)
  : osgQOpenGLWidget(arguments, parent)
  , args_(arguments)
{
    connect(this, &osgQOpenGLWidget::initialized, this, &OsgWidget::finishInit);
    connect(this, &OsgWidget::msg, this, &OsgWidget::showMessage);
}
void OsgWidget::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_P){
        emit msg("key p");
    }
    m_renderer->keyPressEvent(event);
}
void OsgWidget::finishInit()
{
    osg::ArgumentParser& arguments = *args_;
    unsigned int helpType = 0;

    if((helpType = arguments.readHelpType()))
    {
        arguments.getApplicationUsage()->write(std::cout, helpType);
        return ;
    }

    // report any errors if they have occurred when parsing the program arguments.
    if(arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return ;
    }

    if(arguments.argc() <= 1)
    {
        arguments.getApplicationUsage()->write(std::cout,
        osg::ApplicationUsage::COMMAND_LINE_OPTION);
        return ;
    }

    std::string url, username, password;

    while(arguments.read("--login", url, username, password))
    {
        osgDB::Registry::instance()->getOrCreateAuthenticationMap()->addAuthenticationDetails(
            url,
            new osgDB::AuthenticationDetails(username, password)
        );
    }

    std::string device;

    while(arguments.read("--device", device))
    {
        osg::ref_ptr<osgGA::Device> dev = osgDB::readRefFile<osgGA::Device>(device);

        if(dev.valid())
        {
            getOsgViewer()->addDevice(dev);
        }
    }

    // set up the camera manipulators.
    {
        osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

        keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
        keyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
        keyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
        keyswitchManipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator());
        keyswitchManipulator->addMatrixManipulator('5', "Orbit", new osgGA::OrbitManipulator());
        keyswitchManipulator->addMatrixManipulator('6', "FirstPerson", new osgGA::FirstPersonManipulator());
        keyswitchManipulator->addMatrixManipulator('7', "Spherical", new osgGA::SphericalManipulator());

        std::string pathfile;
        double animationSpeed = 1.0;

        while(arguments.read("--speed", animationSpeed)) {}
        char keyForAnimationPath = '8';

        while(arguments.read("-p", pathfile))
        {
            osgGA::AnimationPathManipulator* apm = new osgGA::AnimationPathManipulator(
                pathfile);

            if(apm || !apm->valid())
            {
                apm->setTimeScale(animationSpeed);

                unsigned int num = keyswitchManipulator->getNumMatrixManipulators();
                keyswitchManipulator->addMatrixManipulator(keyForAnimationPath, "Path", apm);
                keyswitchManipulator->selectMatrixManipulator(num);
                ++keyForAnimationPath;
            }
        }

        getOsgViewer()->setCameraManipulator(keyswitchManipulator.get());
    }

    // add the state manipulator
    getOsgViewer()->addEventHandler(new osgGA::StateSetManipulator(getOsgViewer()->getCamera()->getOrCreateStateSet()));

    // add the thread model handler
    getOsgViewer()->addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    getOsgViewer()->addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    getOsgViewer()->addEventHandler(new osgViewer::StatsHandler);

    // add the help handler
    getOsgViewer()->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));

    // add the record camera path handler
    getOsgViewer()->addEventHandler(new osgViewer::RecordCameraPathHandler);

    // add the LOD Scale handler
    getOsgViewer()->addEventHandler(new osgViewer::LODScaleHandler);

    // add the screen capture handler
    getOsgViewer()->addEventHandler(new osgViewer::ScreenCaptureHandler);

    // load the data
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readRefNodeFiles(arguments);

    if(!loadedModel)
    {
        std::cout << arguments.getApplicationName() << ": No data loaded" << std::endl;
        return ;
    }
    else{
        emit msg("model loaded");
    }

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();

    // report any errors if they have occurred when parsing the program arguments.
    if(arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return ;
    }


    // optimize the scene graph, remove redundant nodes and state etc.
    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel);

    getOsgViewer()->setSceneData(loadedModel);

    //        getOsgViewer()->realize();

    return ;
}
void OsgWidget::showMessage(const QString& str)
{
    QMessageBox::information(this, tr("操作提示"), tr("Widget %1").arg(str));
}
