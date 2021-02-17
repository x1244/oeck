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

    // Group to hold all our annotation elements.
    osg::Group* annoGroup = new osg::Group();
    MapNode::get(node)->addChild( annoGroup );

    // Make a group for labels
    osg::Group* labelGroup = new osg::Group();
    annoGroup->addChild( labelGroup );


    // Style our labels:
    Style labelStyle;
    labelStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
    labelStyle.getOrCreate<TextSymbol>()->fill()->color() = Color::Yellow;


    // A lat/long SRS for specifying points.
    const SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();


    //--------------------------------------------------------------------

    // A series of place nodes (an icon with a text label)
    {
        Style pm;
        pm.getOrCreate<IconSymbol>()->url()->setLiteral( "../data/placemark32.png" );
        pm.getOrCreate<IconSymbol>()->declutter() = true;
        pm.getOrCreate<TextSymbol>()->halo() = Color("#5f5f5f");
        pm.getOrCreate<TextSymbol>()->encoding()=TextSymbol::ENCODING_UTF8;
        pm.getOrCreate<TextSymbol>()->font()="simhei.ttf";

        // bunch of pins:
        labelGroup->addChild( new PlaceNode(GeoPoint(geoSRS, -74.00, 40.71), "New York"      , pm));

        labelGroup->addChild( new PlaceNode(GeoPoint(geoSRS, 116.42472, 39.90556), "北京" , pm));

        // test with an LOD:
        osg::LOD* lod = new osg::LOD();
        lod->addChild( new PlaceNode(GeoPoint(geoSRS, 14.68, 50.0), "Prague", pm), 0.0, 2e6);
        labelGroup->addChild( lod );

        // absolute altitude:
        labelGroup->addChild( new PlaceNode(GeoPoint(geoSRS, -87.65, 41.90, 1000, ALTMODE_ABSOLUTE), "Chicago", pm));
    }

    //--------------------------------------------------------------------
    ModelNode *mm = nullptr;
    // a model node with auto scaling.
    {
        struct C : public osg::NodeCallback {
            C(const SpatialReference* geoSRS):srs(geoSRS){}
            void operator()(osg::Node* n, osg::NodeVisitor* nv) {
                static int i=0;
                static double x = -90.;
                i++;
                if(i%30 == 0){
                    x += 0.1;
                    static_cast<ModelNode*>(n)->setPosition(GeoPoint(srs, x, 10., 50000, ALTMODE_ABSOLUTE));
                }
                traverse(n, nv);
            }
        private:
            const SpatialReference* srs;
            
        };
        Style style;
        style.getOrCreate<ModelSymbol>()->autoScale() = true;
        style.getOrCreate<ModelSymbol>()->url()->setLiteral("../data/cessna.osg.500.scale");
//        style.getOrCreate<ModelSymbol>()->url()->setLiteral("../data/cow.osg.5000.scale");
//        style.getOrCreate<ModelSymbol>()->url()->setLiteral("../data/boxman.osg.50.scale");
        ModelNode* modelNode = new ModelNode(mapNode, style); 
        mm = modelNode;
        modelNode->setPosition(GeoPoint(geoSRS, -90., 10., 50000, ALTMODE_ABSOLUTE));
        //通过style添加模型，不需要再设置染色器
//        osgEarth::Registry::shaderGenerator().run(modelNode);
        //modelNode->addCullCallback(new C(geoSRS));
        annoGroup->addChild(modelNode);
    }

	//添加模型
	{
		osg::Node* model = osgDB::readNodeFile("../data/cow.osgt");
        //通过原始模型添加，需要给模型一个染色器
        osgEarth::Registry::shaderGenerator().run(model);
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
		annoGroup->addChild(mt);
	}


    // A path using great-circle interpolation.
    // Keep a pointer to it so we can modify it later on.
    FeatureNode* pathNode = 0;
    {
        struct C : public osg::NodeCallback {
            C(Geometry* p, const SpatialReference* srs)
                : path(p)
            , geoSRS(srs){}
            void operator()(osg::Node* n, osg::NodeVisitor* nv) {
                static int i=0;
                static int ix = 0;
                ++i;
                if (i % 2 == 0){
                    ++ix;
                    if(ix >= path->size()){
                        ix = 0;
                    }
                    const osg::Vec3d& px = path->at(ix);
					auto mm = static_cast<ModelNode*>(n);
                    mm->setPosition(GeoPoint(geoSRS, px.x(), px.y(), px.z(), ALTMODE_ABSOLUTE));
					if(ix < 200){
						osg::Quat quatz(osg::DegreesToRadians(-90.), osg::Z_AXIS);
                        osg::Quat quaty(osg::DegreesToRadians(12.5), osg::Y_AXIS);
						mm->setLocalRotation(quatz*quaty);
					}
					else{
						osg::Quat quatz(osg::DegreesToRadians(180.), osg::Z_AXIS);
                        osg::Quat quaty(osg::DegreesToRadians(-23.), osg::X_AXIS);
						mm->setLocalRotation(quatz*quaty);
					}
                }
                traverse(n, nv);
            }
        private:
            Geometry* path;
            const SpatialReference* geoSRS;
        };
        Geometry* path = new LineString();
        for(int ix = 0; ix < 200; ++ix){
            path->push_back(osg::Vec3d(124.0 - ix/10., 38., 50000. + ix*1000));
        }
        for(int ix = 0; ix < 100; ++ix){
            path->push_back(osg::Vec3d(104.0, 38. + ix/10., 250000 - ix*2000));
        }

        mm->addCullCallback(new C(path, geoSRS));

        Feature* pathFeature = new Feature(path, geoSRS);
//        pathFeature->geoInterp() = GEOINTERP_GREAT_CIRCLE;

        Style pathStyle;
        pathStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::White;
        pathStyle.getOrCreate<LineSymbol>()->stroke()->width() = 1.0f;
        pathStyle.getOrCreate<LineSymbol>()->stroke()->smooth() = true;
        pathStyle.getOrCreate<LineSymbol>()->tessellationSize() = 75000;
        pathStyle.getOrCreate<PointSymbol>()->size() = 8;
        pathStyle.getOrCreate<PointSymbol>()->fill()->color() = Color::Red;
        pathStyle.getOrCreate<PointSymbol>()->smooth() = true;
//        pathStyle.getOrCreate<AltitudeSymbol>()->clamping() = AltitudeSymbol::CLAMP_TO_TERRAIN;
        pathStyle.getOrCreate<AltitudeSymbol>()->technique() = AltitudeSymbol::TECHNIQUE_GPU;
        pathStyle.getOrCreate<RenderSymbol>()->depthOffset()->enabled() = true;

        //OE_INFO << "Path extent = " << pathFeature->getExtent().toString() << std::endl;

        pathNode = new FeatureNode(pathFeature, pathStyle);
        annoGroup->addChild( pathNode );

    }

    viewer.setSceneData( root );
    earthManipulator->setViewpoint(Viewpoint("", 116, 40, 10000.0, -2.50, -90.0, 1.5e6));
    return viewer.run();
}
#ifdef XXX
<model name ="model" driver="simple">
    <url>../data/red_flag.osg.100,100,100.scale</url>
    <location>-74.018 40.717 10</location>
</model>
#endif