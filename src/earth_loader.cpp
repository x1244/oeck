/* -*-c++-*- */
/* osgEarth - Geospatial SDK for OpenSceneGraph
* Copyright 2020 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "earth_loader.h"
#include <osgEarth/LatLongFormatter>
#include <osgEarth/MGRSFormatter>
#include <osgEarth/MouseCoordsTool>
#include <osgEarth/Shadowing>
#include <osgEarth/ActivityMonitorTool>
#include <osgEarth/LogarithmicDepthBuffer>
#include <osgEarth/SimpleOceanLayer>

#include <osgEarth/AnnotationData>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/NodeUtils>
#include <osgEarth/GLUtils>
#include <osgEarth/CullingUtils>

#include <osgEarthDrivers/kml/KML>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>

#define KML_PUSHPIN_URL "../data/placemark32.png"

#define VP_MIN_DURATION      2.0     // minimum fly time.
#define VP_METERS_PER_SECOND 2500.0  // fly speed
#define VP_MAX_DURATION      2.0 //8.0     // maximum fly time.

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace osgEarth::Contrib;

namespace ui = osgEarth::Util::Controls;

//------------------------------------------------------------------------

/** Shared event handlers. */
namespace
{
    void flyToViewpoint(EarthManipulator* manip, const Viewpoint& vp)
    {
        Viewpoint currentVP = manip->getViewpoint();
        double distance = currentVP.focalPoint()->distanceTo(currentVP.focalPoint().get());
        double duration = osg::clampBetween(distance / VP_METERS_PER_SECOND, VP_MIN_DURATION, VP_MAX_DURATION);
        manip->setViewpoint( vp, duration );
    }


    // flies to a viewpoint in response to control event (click)
    struct ClickViewpointHandler : public ControlEventHandler
    {
        ClickViewpointHandler( const Viewpoint& vp, osgGA::CameraManipulator* manip )
            : _vp(vp), _manip( dynamic_cast<EarthManipulator*>(manip) ) { }

        Viewpoint         _vp;
        EarthManipulator* _manip;

        virtual void onClick( class Control* control )
        {
            if ( _manip )
                flyToViewpoint(_manip, _vp);
        }
    };


    // toggles a node in response to a control event (checkbox)
    struct ToggleNodeHandler : public ControlEventHandler
    {
        ToggleNodeHandler( osg::Node* node ) : _node(node) { }

        virtual void onValueChanged( class Control* control, bool value )
        {
            osg::ref_ptr<osg::Node> safeNode = _node.get();
            if ( safeNode.valid() )
                safeNode->setNodeMask( value ? ~0 : 0 );
        }

        osg::observer_ptr<osg::Node> _node;
    };

    /**
     * Toggles the main control canvas on and off.
     */
    struct ToggleCanvasEventHandler : public osgGA::GUIEventHandler
    {
        ToggleCanvasEventHandler(osg::Node* canvas, char key) :
            _canvas(canvas), _key(key)
        {
        }

        bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
        {
            if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
            {
                if (ea.getKey() == _key)
                {
                    osg::ref_ptr< osg::Node > safeNode = _canvas.get();
                    if (safeNode.valid())
                    {
                        safeNode->setNodeMask( safeNode->getNodeMask() ? 0 : ~0 );
                    }
                    return true;
                }
            }
            return false;
        }

        osg::observer_ptr<osg::Node> _canvas;
        char _key;
    };

    // sets a user-specified uniform.
    struct ApplyValueUniform : public ControlEventHandler
    {
        osg::ref_ptr<osg::Uniform> _u;
        ApplyValueUniform(osg::Uniform* u) :_u(u) { }
        void onValueChanged(Control* c, double value)
        {
            _u->set( float(value) );
            osgEarth::Registry::instance()->dataStore().store(
                osgEarth::Registry::instance(), _u->getName(), _u.get());
        }
    };

    struct ToggleDefine : public ControlEventHandler
    {
        osg::ref_ptr<osg::StateSet> _ss;
        std::string _name;
        ToggleDefine(osg::StateSet* ss, const std::string& name) : _ss(ss), _name(name) { }
        void onValueChanged(Control* c, bool value) {
            if (value) _ss->setDefine(_name, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
            else _ss->setDefine(_name, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE); //(_name);
        }
    };
}




//------------------------------------------------------------------------

#undef  LC
#define LC "[EarthLoader] "

osg::Group*
EarthLoader::load(osg::ArgumentParser&   args,
                    osgViewer::ViewerBase* viewer,
                    Container*             userContainer,
                    const osgDB::Options*  readOptions) const
{
    osg::ref_ptr<osgDB::Options> myReadOptions = Registry::cloneOrCreateOptions(readOptions);

    // read in the Earth file:
    osg::ref_ptr<osg::Node> node = osgDB::readNodeFiles(args, myReadOptions.get());

    // fallback in case none is specified:
    if (!node.valid())
    {
        OE_WARN << LC << "No valid earth file loaded - aborting" << std::endl;
        return NULL;
    }

    osg::ref_ptr<MapNode> mapNode = MapNode::get(node.get());

    if (args.read("--tessellation") || args.read("--tess"))
    {
        mapNode->getTerrainOptions().setGPUTessellation(true);
    }

    if ( !mapNode.valid() )
    {
        OE_WARN << LC << "Loaded scene graph does not contain a MapNode - aborting" << std::endl;
        return 0L;
    }

    // collect the views
    osgViewer::Viewer::Views views;
    if (viewer)
    {
        viewer->getViews(views);
    }

    // warn about not having an earth manip
    for (osgViewer::Viewer::Views::iterator view = views.begin(); view != views.end(); ++view)
    {
        EarthManipulator* manip = dynamic_cast<EarthManipulator*>((*view)->getCameraManipulator());
        if ( manip == 0L )
        {
            OE_WARN << LC << "Helper used before installing an EarthManipulator" << std::endl;
        }
    }

    // a root node to hold everything:
    osg::Group* root = new osg::Group();

    root->addChild( node );
    
    // open the map node:
    if (!mapNode->open())
    {
        OE_WARN << LC << "Failed to open MapNode" << std::endl;
        return 0L;
    }

    // parses common cmdline arguments and apply to the first view:
    if ( !views.empty() )
    {
        parse( mapNode.get(), args, views.front(), root, userContainer );
        
        float lodscale;
        if (args.read("--lodscale", lodscale))
        {
            LODScaleGroup* g = new LODScaleGroup();
            g->setLODScaleFactor(osg::maximum(lodscale, 0.0001f));
            osgEarth::insertGroup(g, mapNode->getParent(0));
            OE_NOTICE << "LOD Scale set to: " << lodscale << std::endl;
        }
    }

    // configures each view with some stock goodies
    for (osgViewer::Viewer::Views::iterator view = views.begin(); view != views.end(); ++view)
    {
        configureView( *view );
    }

    // vsync on/off?
    optional<bool> vsync;
    if (args.read("--vsync"))
        vsync = true;
    else if (args.read("--novsync"))
        vsync = false;

    if (viewer)
    {
#ifdef OSG_GL3_AVAILABLE
        GL3RealizeOperation* rop = new GL3RealizeOperation();
#else
        CustomRealizeOperation* rop = new GL3RealizeOperation();
#endif
        if (vsync.isSet())
            rop->setSyncToVBlank(vsync.get());

        viewer->setRealizeOperation(rop);
    }

    return root;
}


void
EarthLoader::parse(MapNode*             mapNode,
                     osg::ArgumentParser& args,
                     osgViewer::View*     view,
                     osg::Group*          root,
                     LabelControl*        userLabel ) const
{
    VBox* vbox = new VBox();
    vbox->setAbsorbEvents( true );
    vbox->setBackColor( Color(Color::Black, 0.8) );
    vbox->setHorizAlign( Control::ALIGN_LEFT );
    vbox->setVertAlign( Control::ALIGN_BOTTOM );
    vbox->addControl( userLabel );

    parse(mapNode, args, view, root, vbox);
}

void
EarthLoader::parse(MapNode*             mapNode,
                     osg::ArgumentParser& args,
                     osgViewer::View*     view,
                     osg::Group*          root,
                     Container*           userContainer ) const
{
    if ( !root )
        root = mapNode;

    // options to use for the load
    osg::ref_ptr<osgDB::Options> dbOptions = Registry::instance()->cloneOrCreateOptions();

    // parse out custom example arguments first:
    bool useCoords     = args.read("--coords");
    bool showActivity  = args.read("--activity");
    bool useLogDepth2  = args.read("--logdepth2");
    bool useLogDepth   = !args.read("--nologdepth") && !useLogDepth2; //args.read("--logdepth");
    bool kmlUI         = args.read("--kmlui");

    std::string kmlFile;
    args.read( "--kml", kmlFile );

    // animation path:
    std::string animpath;
    if ( args.read("--path", animpath) )
    {
        view->setCameraManipulator( new osgGA::AnimationPathManipulator(animpath) );
    }

    // vertical field of view:
    float vfov = -1.0f;
    if (args.read("--vfov", vfov) && vfov > 0.0f)
    {
        double fov, ar, n, f;
        view->getCamera()->getProjectionMatrixAsPerspective(fov, ar, n, f);
        view->getCamera()->setProjectionMatrixAsPerspective(vfov, ar, n, f);
    }

    // Install a new Canvas for our UI controls, or use one that already exists.
    ControlCanvas* canvas = ControlCanvas::getOrCreate( view );

    Container* mainContainer;
    if ( userContainer )
    {
        mainContainer = userContainer;
    }
    else
    {
        mainContainer = new VBox();
        mainContainer->setAbsorbEvents( false );
        mainContainer->setBackColor( Color(Color::Black, 0.8) );
        mainContainer->setHorizAlign( Control::ALIGN_LEFT );
        mainContainer->setVertAlign( Control::ALIGN_BOTTOM );
    }
    canvas->addControl( mainContainer );

    // Add an event handler to toggle the canvas with a key press;
    //view->addEventHandler(new ToggleCanvasEventHandler(canvas, 'y'));

    // look for external data in the map node:
    const Config externals = mapNode ? mapNode->externalConfig() : Config();
#ifdef KMLKML
    // Loading KML from the command line:
    if ( !kmlFile.empty() && mapNode )
    {
        KML::KMLOptions kml_options;
        kml_options.declutter() = true;

        // set up a default icon for point placemarks:
        IconSymbol* defaultIcon = new IconSymbol();
        defaultIcon->url()->setLiteral(KML_PUSHPIN_URL);
        kml_options.defaultIconSymbol() = defaultIcon;

        TextSymbol* defaultText = new TextSymbol();
        defaultText->halo() = Stroke(0.3,0.3,0.3,1.0);
        kml_options.defaultTextSymbol() = defaultText;

        osg::Node* kml = KML::load( URI(kmlFile), mapNode, kml_options );
        if ( kml )
        {
            if (kmlUI)
            {
                Control* c = AnnotationGraphControlFactory().create(kml, view);
                if ( c )
                {
                    c->setVertAlign( Control::ALIGN_TOP );
                    mainContainer->addControl( c );
                }
            }
            mapNode->addChild( kml );
        }
        else
        {
            OE_NOTICE << "Failed to load " << kmlFile << std::endl;
        }
    }
#endif
    // Configure the mouse coordinate readout:
    if ( useCoords && mapNode )
    {
        LabelControl* readout = new LabelControl();
        readout->setBackColor( Color(Color::Black, 0.8) );
        readout->setHorizAlign( Control::ALIGN_RIGHT );
        readout->setVertAlign( Control::ALIGN_BOTTOM );
    
        Formatter* formatter = new LatLongFormatter(LatLongFormatter::FORMAT_DECIMAL_DEGREES); //DEGREES_MINUTES_SECONDS_TERSE);    
        MouseCoordsTool* mcTool = new MouseCoordsTool( mapNode );
        mcTool->addCallback( new MouseCoordsLabelCallback(readout, formatter) );
        view->addEventHandler( mcTool );

        canvas->addControl( readout );
    }


    // Add the credits display
    if (mapNode)
    {
        //canvas->addControl(AttributionControlFactory().create(mapNode));
    }

    // Configure for an ortho camera:
    if ( args.read("--ortho") )
    {
        EarthManipulator* em = dynamic_cast<EarthManipulator*>(view->getCameraManipulator());
        if (em)
        {
            double V, A, N, F;
            view->getCamera()->getProjectionMatrixAsPerspective(V, A, N, F);
            em->setInitialVFOV( V );
        }

        view->getCamera()->setProjectionMatrixAsOrtho(-1, 1, -1, 1, 0, 1);
    }

    // activity monitor (debugging)
    if ( showActivity )
    {
        VBox* vbox = new VBox();
        vbox->setBackColor( Color(Color::Black, 0.8) );
        vbox->setHorizAlign( Control::ALIGN_RIGHT );
        vbox->setVertAlign( Control::ALIGN_BOTTOM );
        view->addEventHandler( new ActivityMonitorTool(vbox) );
        canvas->addControl( vbox );
    }

    // Install logarithmic depth buffer on main camera
    if ( useLogDepth )
    {
        OE_INFO << LC << "Activating logarithmic depth buffer (vertex-only) on main camera" << std::endl;
        osgEarth::Util::LogarithmicDepthBuffer logDepth;
        logDepth.setUseFragDepth( false );
        logDepth.install( view->getCamera() );
    }

    else if ( useLogDepth2 )
    {
        OE_INFO << LC << "Activating logarithmic depth buffer (precise) on main camera" << std::endl;
        osgEarth::Util::LogarithmicDepthBuffer logDepth;
        logDepth.setUseFragDepth( true );
        logDepth.install( view->getCamera() );
    }

    // Generic named value uniform with min/max.
    VBox* uniformBox = 0L;
    while( args.find( "--uniform" ) >= 0 )
    {
        std::string name;
        float minval, maxval;
        if ( args.read( "--uniform", name, minval, maxval ) )
        {
            if ( uniformBox == 0L )
            {
                uniformBox = new VBox();
                uniformBox->setBackColor(0,0,0,0.5);
                uniformBox->setAbsorbEvents( true );
                mainContainer->addControl( uniformBox );
            }
            osg::Uniform* uniform = new osg::Uniform(osg::Uniform::FLOAT, name);
            uniform->set( minval );
            root->getOrCreateStateSet()->addUniform( uniform, osg::StateAttribute::OVERRIDE );
            HBox* box = new HBox();
            box->addControl( new LabelControl(name) );
            HSliderControl* hs = box->addControl( new HSliderControl(minval, maxval, minval, new ApplyValueUniform(uniform)));
            hs->setHorizFill(true, 200);
            box->addControl( new LabelControl(hs) );
            uniformBox->addControl( box );
            OE_INFO << LC << "Installed uniform controller for " << name << std::endl;
        }
    }

    while (args.find("--define") >= 0)
    {
        std::string name;
        if (args.read("--define", name))
        {
            if ( uniformBox == 0L )
            {
                uniformBox = new VBox();
                uniformBox->setBackColor(0,0,0,0.5);
                uniformBox->setAbsorbEvents( true );
                mainContainer->addControl( uniformBox );
            }
            
            HBox* box = new HBox();
            box->addControl(new CheckBoxControl(false, new ToggleDefine(mapNode->getOrCreateStateSet(), name)));
            box->addControl(new LabelControl(name));
            uniformBox->addControl(box);
        }
    }

    // Map inspector:
    if (args.read("--inspect") && mapNode)
    {
        mapNode->addExtension( Extension::create("mapinspector", ConfigOptions()) );
    }

    // Memory monitor:
    if (args.read("--monitor") && mapNode)
    {
        mapNode->addExtension(Extension::create("monitor", ConfigOptions()) );
    }

    // Simple sky model:
    if (args.read("--sky") && mapNode)
    {
        mapNode->open(); // necessary to resolve the SRS on the next line
        std::string ext = mapNode->getMapSRS()->isGeographic() ? "sky_simple" : "sky_gl";
        mapNode->addExtension(Extension::create(ext, ConfigOptions()) );
    }
#ifdef OCEANOCEAN
    // Simple ocean model:
    if (args.read("--ocean") && mapNode)
    {
        SimpleOceanLayer* layer = new SimpleOceanLayer();
        mapNode->getMap()->addLayer(layer);
        Control* ui = OceanControlFactory::create(layer);
        mainContainer->addControl(ui);
    }
#endif
    // Arbitrary extension:
    std::string extname;
    if (args.read("--extension", extname) && mapNode)
    {
        Extension* ext = Extension::create(extname, ConfigOptions());
        if (ext)
            mapNode->addExtension(ext);
    }


    // Hook up the extensions!
    if (mapNode)
    {
        for(std::vector<osg::ref_ptr<Extension> >::const_iterator eiter = mapNode->getExtensions().begin();
            eiter != mapNode->getExtensions().end();
            ++eiter)
        {
            Extension* e = eiter->get();

            // Check for a View interface:
            ExtensionInterface<osg::View>* viewIF = ExtensionInterface<osg::View>::get( e );
            if ( viewIF )
                viewIF->connect( view );

            // Check for a Control interface:
            ExtensionInterface<Control>* controlIF = ExtensionInterface<Control>::get( e );
            if ( controlIF )
                controlIF->connect( mainContainer );
        }
    }

    // Shadowing. This is last because it needs access to a light which may be provided
    // by one of the Sky extensions.
    if (args.read("--shadows") && mapNode)
    {
        int unit;
        if ( mapNode->getTerrainEngine()->getResources()->reserveTextureImageUnit(unit, "ShadowCaster") )
        {
            ShadowCaster* caster = new ShadowCaster();
            caster->setTextureImageUnit( unit );
            caster->setLight( view->getLight() );
            caster->getShadowCastingGroup()->addChild( mapNode->getLayerNodeGroup() );
            caster->getShadowCastingGroup()->addChild(mapNode->getTerrainEngine());
            if ( mapNode->getNumParents() > 0 )
            {
                osgEarth::insertGroup(caster, mapNode->getParent(0));
            }
            else
            {
                caster->addChild(mapNode);
                root = caster;
            }
        }
    }

    root->addChild( canvas );
}


void
EarthLoader::configureView( osgViewer::View* view ) const
{
    // default uniform values:
    GLUtils::setGlobalDefaults(view->getCamera()->getOrCreateStateSet());

    // disable small feature culling (otherwise Text annotations won't render)
    view->getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    // instruct the database pager to not modify the unref settings
    view->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, false);

    // thread-safe initialization of the OSG wrapper manager. Calling this here
    // prevents the "unsupported wrapper" messages from OSG
    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");

    // add some stock OSG handlers:
    view->addEventHandler(new osgViewer::StatsHandler());
    view->addEventHandler(new osgViewer::WindowSizeHandler());
    view->addEventHandler(new osgViewer::ThreadingHandler());
    view->addEventHandler(new osgViewer::LODScaleHandler());
    view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));
    view->addEventHandler(new osgViewer::RecordCameraPathHandler());
    view->addEventHandler(new osgViewer::ScreenCaptureHandler());
}


std::string
EarthLoader::usage() const
{
    return Stringify()
        << "  --sky                         : add a sky model\n"
        << "  --kml <file.kml>              : load a KML or KMZ file\n"
        << "  --kmlui                       : display a UI for toggling nodes loaded with --kml\n"
        << "  --coords                      : display map coords under mouse\n"
        << "  --ortho                       : use an orthographic camera\n"
        << "  --logdepth                    : activates the logarithmic depth buffer\n"
        << "  --logdepth2                   : activates logarithmic depth buffer with per-fragment interpolation\n"
        << "  --shadows                     : activates model layer shadows\n"
        << "  --out-earth [file]            : write the loaded map to an earth file\n"
        << "  --uniform [name] [min] [max]  : create a uniform controller with min/max values\n"
        << "  --define [name]               : install a shader #define\n"
        << "  --path [file]                 : load and playback an animation path\n"
        << "  --extension [name]            : loads a named extension\n"
        << "  --ocean                       : add a simple ocean model (requires bathymetry)\n";
}

