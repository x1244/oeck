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

#include <osgViewer/Viewer>
#include <osgEarth/Controls>

#include <osgEarth/Notify>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/FeatureNode>
#include <osgEarth/GeometryFactory>
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
#include <iostream>
#include "MouseIntersection.h"

#define LC "[viewer] "

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace std;
int
usage(const char* name)
{
    OE_NOTICE 
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}


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
//    viewer.getCamera()->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    
    osg::Group* root = new osg::Group();
    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags    
    osg::Node* node = MapNodeHelper().load(arguments, &viewer);
    if ( node )
    {
        root->addChild(node);
        //return Metrics::run(viewer);
    }
    else
    {
        return usage(argv[0]);
    }
    
    // find the map node that we loaded.
    MapNode* mapNode = MapNode::findMapNode(node);
    if ( !mapNode )
        return usage(argv[0]);
    auto cav = Controls::ControlCanvas::getOrCreate(&viewer);
	//no root
	Controls::LabelControl* label = new Controls::LabelControl;
	label->setSize(800, 50);
	label->setMargin(10);
	label->setHorizAlign(Controls::Control::ALIGN_LEFT);
	label->setVertAlign(Controls::Control::ALIGN_TOP);
	label->setBackColor(0, 0, 0, 0.5);
	label->setText("text here");
	cav->addControl(label);
	#ifdef XZZ
	#endif
    viewer.setSceneData( root );
    //earthManipulator->setViewpoint(Viewpoint("", 116, 40, 10000.0, -2.50, -90.0, 1.5e6));
    return viewer.run();
}
#ifdef XXX
<model name ="model" driver="simple">
    <url>../data/red_flag.osg.100,100,100.scale</url>
    <location>-74.018 40.717 10</location>
</model>
#endif