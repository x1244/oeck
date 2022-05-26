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

#include <osg/ref_ptr>
#include <osg/Image>
#include <osg/LineWidth>
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
#include <osgEarth/Style>
#include <osgEarth/OGRFeatureSource>
#include <osgEarth/FeatureModelLayer>
#include <osgEarth/FeatureImageLayer>
#include <osgEarth/PointSymbol>
#include <osgDB/ReadFile>
#include <iostream>

#include <osgEarth/Metrics>

#define LC "[viewer] "

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace std;
int usage(const char* name)
{
    OE_NOTICE 
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}

class PointHandler : public osgGA::GUIEventHandler
{
public:
explicit PointHandler(MapNode* root)
    : root_(root), lmd_(-83.), b_(9.){

    const SpatialReference* geoSRS = root_->getMapSRS()->getGeographicSRS();
    Style style;
    PointSymbol* ls = style.getOrCreateSymbol<PointSymbol>();
    ls->size() = 8;
    ls->fill()->color() = Color::Red;
    ls->smooth() == true;
    AltitudeSymbol* alt = style.getOrCreate<AltitudeSymbol>();
    alt->clamping() = alt->CLAMP_TO_TERRAIN;
    alt->technique() = alt->TECHNIQUE_GPU;
    style.getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;

    auto px = new osgEarth::Point;
    px->set(osg::Vec3d(lmd_, b_, 0.));
    Feature* feature = new Feature(px, geoSRS);
    FeatureNode* node = new FeatureNode(feature, style);
    ff_ = node;
    auto g = new osg::Group;
    g->addChild(node);
    root_->addChild(g);
}
bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa){
    if (ea.getHandled()) return false;

    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::KEYUP):
        {
//            cout <<"key: " <<ea.getKey() <<endl;
            if (ea.getKey() == 'p' ) {
                lmd_ -= .2;
                b_ += .1;
                const SpatialReference* geoSRS = \
                         root_->getMapSRS()->getGeographicSRS();
                auto px = new osgEarth::Point;
                px->set(osg::Vec3d(lmd_, b_, 0.));
                Feature* feature = new Feature(px, geoSRS);
                ff_->getFeatures().push_back(feature);
                ff_->dirty();
//                cout <<"FF: " <<ff_->getFeatures().size() <<endl;
            }
            break;
        }
    default:
        break;
    }
    return false;
}
private:
osgEarth::MapNode* root_;
osgEarth::FeatureNode* ff_;
double lmd_;
double b_;
};



int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);

    // help?
    if ( arguments.read("--help") )
        return usage(argv[0]);
    // create a viewer:
    osgViewer::Viewer viewer(arguments);

    // Tell the database pager to not modify the unref settings
    viewer.getDatabasePager()->setUnrefImageDataAfterApplyPolicy( true, false );

    // thread-safe initialization of the OSG wrapper manager. Calling this here
    // prevents the "unsupported wrapper" messages from OSG
    osgDB::Registry::instance()->getObjectWrapperManager()->findWrapper("osg::Image");

    // install our default manipulator (do this before calling load)
    osg::ref_ptr<osgEarth::Util::EarthManipulator> earthManipulator = new EarthManipulator(arguments);
    viewer.setCameraManipulator(earthManipulator);

    // disable the small-feature culling
    viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    // set a near/far ratio that is smaller than the default. This allows us to get
    // closer to the ground without near clipping. If you need more, use --logdepth
    viewer.getCamera()->setNearFarRatio(0.0001);
    viewer.getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    
    osg::ref_ptr<osg::Group> root = new osg::Group;
    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags   
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(argv[1]);
	root->addChild(node);

    // find the map node that we loaded.
    MapNode* mapNode = MapNode::findMapNode(node);
    if ( !mapNode )
        return usage(argv[0]);
    osg::ref_ptr<PointHandler> ph = new PointHandler(mapNode);
    viewer.addEventHandler(ph);
    viewer.setSceneData( root );
	viewer.realize();  
    return viewer.run();
}
