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
#include <osgDB/ReadFile>
#include <iostream>

#include <osgEarth/Metrics>
#include <iostream>
#include "earth_loader.h"
#include "panorama.h"
#include "panorama_camera.h"
#include "pick.h"

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
void setCanvas(osgViewer::Viewer* v);
osg::Camera* createBackground(std::string strImg);

osg::Camera* createForeground(std::string strImg);
osg::MatrixTransform* createHud(std::string str);
osg::Camera* hudCamera();
double getHeight(MapNode* map);
//#define TEX 1
#define XTT 1
#ifdef XTT
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
	//osg::Node* model = osgDB::readNodeFile("../data/cow.osgt");
	//root->addChild(model);

    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags   
#define EARTH 1
#ifdef EARTH
//#define YYY 1	
#ifdef YYY
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(argv[1]);
	root->addChild(node);
#else
    osg::Node* node = MapNodeHelper().load(arguments, &viewer);
	cout <<"earth r:" <<node->getBound().radius() <<endl;
    if ( node )
    {
        root->addChild(node);
    }
    else
    {
        return usage(argv[0]);
    }
#endif

    // find the map node that we loaded.
    MapNode* mapNode = MapNode::findMapNode(node);
    if ( !mapNode )
        return usage(argv[0]);
#define XXX 1
#ifdef XXX
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
    ModelNode *cessna = nullptr;
    {
        Style style;
        //这里必须设置自动缩放
		//autoScale可以对比cow2没设置的
		//autoScale设置后，无论视点远近，看起来都一样大。
        style.getOrCreate<ModelSymbol>()->autoScale() = true;
        style.getOrCreate<ModelSymbol>()->url()->setLiteral("../data/cessna.osg.1000.scale");
        //style.getOrCreate<ModelSymbol>()->url()->setLiteral("../data/cessna.osg");
        ModelNode* modelNode = new ModelNode(mapNode, style);
        cessna = modelNode;
        modelNode->setPosition(GeoPoint(geoSRS, 121., 38., 50000, ALTMODE_ABSOLUTE));
        annoGroup->addChild(modelNode);
		cout <<"cessna r:" <<modelNode->getBound().radius() <<endl;
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
		geoSRS->getEllipsoid()->computeLocalToWorldTransformFromLatLongHeight(osg::DegreesToRadians(40.0), osg::DegreesToRadians(116.0), 0.0, Lmatrix);
		//放大一些，可以看到
		Lmatrix.preMult(osg::Matrix::scale(osg::Vec3(10000, 10000, 10000)));

		osg::MatrixTransform* mt = new osg::MatrixTransform;
		mt->setMatrix(Lmatrix);
		mt->addChild(model);
		annoGroup->addChild(mt);
		cout <<"cow r:" <<mt->getBound().radius() <<endl;
	}
    {
		//http://docs.osgearth.org/en/latest/faq.html
		//手册上说通过以下方法，可以自动贴合到地形表面,但我没能实现
        Style style;
        //style.getOrCreate<ModelSymbol>()->autoScale() = true;
        style.getOrCreate<ModelSymbol>()->url()->setLiteral("../data/cow.osg.1000.scale");
        ModelNode* modelNode = new ModelNode(mapNode, style);
		GeoTransform* xform = new GeoTransform();
 		cout <<"terrain:" <<mapNode->getTerrain() <<endl;
        xform->setTerrain(mapNode->getTerrain());
        GeoPoint point(geoSRS, 121.3, 38.8);
        xform->setPosition(point);
		xform->addChild(modelNode);
		annoGroup->addChild(xform);
		cout <<"cow2 r:" <<xform->getBound().radius() <<endl;
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
		if(cessna){
            cessna->addUpdateCallback(new C(path, geoSRS));
		}

        Feature* pathFeature = new Feature(path, geoSRS);
//        pathFeature->geoInterp() = GEOINTERP_GREAT_CIRCLE;

        Style pathStyle;
        pathStyle.getOrCreate<LineSymbol>()->stroke()->color() = Color::White;
        pathStyle.getOrCreate<LineSymbol>()->stroke()->width() = 1.0f;
        pathStyle.getOrCreate<LineSymbol>()->stroke()->smooth() = true;
//        pathStyle.getOrCreate<LineSymbol>()->tessellationSize() = 75000;
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
#endif	
#endif//EARTH

	//setCanvas(&viewer);
	//添加背景不成功
	//root->addChild(createBackground("../data/1.png"));
	osg::Camera* fg = createForeground("../data/1.png");
	root->addChild(fg);
	//viewer.addSlave(fg);
/*
	auto mf = createHud("../data/1.png");
	auto cam = hudCamera();
	cam->addChild(mf);
	root->addChild(cam);
*/	
    viewer.setSceneData( root );
    //earthManipulator->setViewpoint(Viewpoint("", 116, 40, 10000.0, -2.50, -90.0, 1.5e6));
	//viewer.addEventHandler(new PickHandler(&viewer));
	viewer.realize();  
	
//    osgEarth::Util::LogarithmicDepthBuffer buf;
//    buf.install( viewer.getCamera() );
    cout <<"height: " <<getHeight(mapNode) <<endl;
    return viewer.run();
}
#endif


#ifdef TEX
int main(int argc, char* argv[])
{
    osg::ref_ptr<osgViewer::Viewer> viewer1 = new osgViewer::Viewer;
    viewer1->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
    osg::ref_ptr<osg::Group> group1 = new osg::Group;
    //group1->addChild(createBackground("../data/1.png"));
	osg::Node* model = osgDB::readNodeFile("../data/cow.osgt");
	group1->addChild(model);
    group1->addChild(createForeground("../data/1.png"));

//	auto mf = createHud("../data/1.png");
//	auto cam = hudCamera();
//	cam->addChild(mf);
//	group1->addChild(cam);

	
    viewer1->setSceneData(group1.get());
    //viewer1->setUpViewInWindow(0, 0, 1920, 1280, 0);
    return viewer1->run();
}
#endif
osg::Camera* createBackground(std::string strImg)
{
    osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry1 = new osg::Geometry;
    osg::ref_ptr<osg::Camera> camera1 = new osg::Camera;

    camera1->setAllowEventFocus(false);
    camera1->setProjectionMatrixAsOrtho2D(0, 1920, 0, 1280);
    //camera1->setViewport(0, 0, 960, 600);

    camera1->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera1->setRenderOrder(osg::Camera::PRE_RENDER);
    camera1->setClearMask(GL_DEPTH_BUFFER_BIT  |  GL_COLOR_BUFFER_BIT);
    //camera1->setClearColor(osg::Vec4());
    camera1->setViewMatrix(osg::Matrix::identity());

    //压入顶点
    osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
    vertex->push_back(osg::Vec3(0.0,0.0,0.0));
    vertex->push_back(osg::Vec3(1920.0, 0.0, 0.0));
    vertex->push_back(osg::Vec3(1920.0, 1280.0, 0.0));
    vertex->push_back(osg::Vec3(0.0, 1280.0, 0.0));
    geometry1->setVertexArray(vertex);

    //压入法线
    osg::ref_ptr<osg::Vec3Array> norml = new osg::Vec3Array;
    norml->push_back(osg::Vec3(0.0, 0.0, 1.0));
    geometry1->setNormalArray(norml);
    geometry1->setNormalBinding(osg::Geometry::BIND_OVERALL);

    //纹理坐标
    osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array;
    coord->push_back(osg::Vec2(0.0,0.0));
    coord->push_back(osg::Vec2(1.0, 0.0));
    coord->push_back(osg::Vec2(1.0, 1.0));
    coord->push_back(osg::Vec2(0.0, 1.0));
    geometry1->setTexCoordArray(0, coord);
    geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    osg::ref_ptr<osg::Image> img1 = osgDB::readImageFile(strImg);
    if (!img1.valid())
    {
        std::cout << "" << std::endl;
    }

    osg::ref_ptr<osg::Texture2D> texture2d = new osg::Texture2D;
    texture2d->setImage(0, img1);
    geometry1->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture2d,osg::StateAttribute::ON);


    camera1->addChild(geode1);
    geode1->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode1->addDrawable(geometry1);

    return camera1.release();
}


void setCanvas(osgViewer::Viewer* v)
{
	auto cav = Controls::ControlCanvas::getOrCreate(v);
	
	osg::ref_ptr<Controls::LabelControl> label = new Controls::LabelControl;
	label->setSize(800, 50);
	label->setMargin(10);
	label->setHorizAlign(Controls::Control::ALIGN_LEFT);
	label->setVertAlign(Controls::Control::ALIGN_TOP);
	label->setBackColor(0, 0, 0, 0.2);
	label->setOpacity(0.8);
	label->setEncoding(osgText::String::ENCODING_UTF8);
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/simhei.ttf");
	label->setFont(font); 
	std::string tx("显示一些文字内容：北京");
	label->setText(tx);
	cav->addControl(label.get());
	
	osg::ref_ptr<osg::Image> m = osgDB::readImageFile("../data/1.png");
	osg::ref_ptr<Controls::ImageControl> image = new Controls::ImageControl(m);
	image->setSize(1800, 200);
	image->setMargin(5);
	image->setHorizAlign(Controls::Control::ALIGN_CENTER);
	image->setVertAlign(Controls::Control::ALIGN_BOTTOM);
	image->setBackColor(0., 0.5, 1., 0.4);
	image->setEventCallback(new Panorama(image.get()));
	
	cav->addControl(image.get());

}
osg::Camera* createForeground(std::string strImg)
{
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 40;
	traits->y = 40;
	traits->width = 800;
	traits->height = 600;
	traits->windowDecoration = true;
	traits->sharedContext = 0;
	
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);

	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry1 = new osg::Geometry;

    //压入顶点
	const double z = -0.1;
    osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
    vertex->push_back(osg::Vec3(0.0,0.0,z));
    vertex->push_back(osg::Vec3(1920.0, 0.0, z));
    vertex->push_back(osg::Vec3(1920.0, 200.0, z));
    vertex->push_back(osg::Vec3(0.0, 200.0, z));
    geometry1->setVertexArray(vertex);

    //压入法线
    osg::ref_ptr<osg::Vec3Array> norml = new osg::Vec3Array;
    norml->push_back(osg::Vec3(0.0, 0.0, 1.0));
    geometry1->setNormalArray(norml);
    geometry1->setNormalBinding(osg::Geometry::BIND_OVERALL);

    //纹理坐标
    osg::ref_ptr<osg::Vec2Array> coord = new osg::Vec2Array;
    coord->push_back(osg::Vec2(0.0, 0.0));
    coord->push_back(osg::Vec2(1.0, 0.0));
    coord->push_back(osg::Vec2(1.0, 1.0));
    coord->push_back(osg::Vec2(0.0, 1.0));
    geometry1->setTexCoordArray(0, coord);
    geometry1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    osg::ref_ptr<osg::Image> img1 = osgDB::readImageFile(strImg);
    if (!img1.valid())
    {
        std::cout <<"load image " <<strImg <<" error" << std::endl;
    }

    osg::ref_ptr<osg::Texture2D> texture2d = new osg::Texture2D;
    texture2d->setImage(0, img1);
    geometry1->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture2d,osg::StateAttribute::ON);

    geode1->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode1->addDrawable(geometry1);
	
	osg::ref_ptr<osg::MatrixTransform> t1 = new osg::MatrixTransform();
	t1->setMatrix(osg::Matrix::translate(-1920,0,0));
	t1->addChild(geode1);
	osg::ref_ptr<osg::MatrixTransform> t2 = new osg::MatrixTransform();
	t2->setMatrix(osg::Matrix::translate(0,0,0));
	t2->addChild(geode1);
	osg::ref_ptr<osg::MatrixTransform> t3 = new osg::MatrixTransform();
	t3->setMatrix(osg::Matrix::translate(1920,0,0));
	t3->addChild(geode1);

	osg::ref_ptr<osg::MatrixTransform> tranf = new osg::MatrixTransform();
	tranf->addChild(t1);
	tranf->addChild(t2);
	tranf->addChild(t3);
	tranf->setUpdateCallback(new PanoramaCamera(tranf.get()));
	//标志-------
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
    osg::ref_ptr<osg::LineWidth> lineWid = new osg::LineWidth(2.0f);
    stateset->setAttribute(lineWid);
	
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    /*v->push_back(osg::Vec3(990,150.0,z + 0.1));
    v->push_back(osg::Vec3(1010,150.0,z + 0.1));
    v->push_back(osg::Vec3(1000,140.0,z + 0.1));
    v->push_back(osg::Vec3(1000.0,160.0,z + 0.1));*/
    v->push_back(osg::Vec3(990,150.0,z + 0.1));
    v->push_back(osg::Vec3(1010,150.0,z + 0.1));
    v->push_back(osg::Vec3(1010,160.0,z + 0.1));
    v->push_back(osg::Vec3(990.0,160.0,z + 0.1));
    geom->setVertexArray(v.get());
    osg::ref_ptr<osg::Vec3Array> n=new osg::Vec3Array();
    n->push_back(osg::Vec3(0.0,0.0,-1.0));
    geom->setNormalArray(n.get());
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array();
    c->push_back(osg::Vec4(1.0,0.0,0.0,0.8));
    c->push_back(osg::Vec4(0.0,0.0,1.0,0.8));
    c->push_back(osg::Vec4(1.0,1.0,0.0,0.8));
    c->push_back(osg::Vec4(0.0,1.0,1.0,0.8));
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));
    osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
    geode2->addDrawable(geom.get());
	osg::ref_ptr<osg::MatrixTransform> tranf2 =new osg::MatrixTransform();
	tranf2->addChild(geode2); 
	//tranf2->setUpdateCallback(new PanoramaCamera(tranf.get()));
	

    osg::ref_ptr<osg::Camera> camera1 = new osg::Camera;
    //camera1->setGraphicsContext(gc);
    camera1->setAllowEventFocus(false);
    camera1->setProjectionMatrixAsOrtho2D(0, 1920, 0, 1280);
    camera1->setViewport(0, 0, 1920, 1280);

    camera1->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera1->setRenderOrder(osg::Camera::POST_RENDER);
    camera1->setClearMask(GL_DEPTH_BUFFER_BIT);
 	camera1->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    camera1->setClearColor(osg::Vec4(0.5, 0.3, 0.2, 0.1));
    camera1->setViewMatrix(osg::Matrix::identity());
	
	osg::ref_ptr<osg::MatrixTransform> tranfx =new osg::MatrixTransform();
	tranfx->addChild(tranf);
	tranfx->addChild(tranf2);
	
    camera1->addChild(tranfx);

    return camera1.release();
}

osg::MatrixTransform* createHud(std::string str)
{
	double r = 1.5, h = -1.0;
	osg::Vec3 corner(-r, -r, h);
	osg::Vec3 w(2*r, 0., 0.);
	osg::Vec3 v(0., 2*r, 0.);
	osg::ref_ptr<osg::Drawable> quad = createTexturedQuadGeometry(corner, w, v);
	
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(str));
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
	quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	
	osg::ref_ptr<osg::Geode> gnode = new osg::Geode;
	gnode->addDrawable(quad);
	osg::ref_ptr<osg::MatrixTransform> mf = new osg::MatrixTransform;
	mf->addChild(gnode);
	return mf.release();
}
osg::Camera* hudCamera()
{
	osg::ref_ptr<osg::Camera> cam = new osg::Camera;
	cam->setViewport(100, 0, 1720, 200); //世界屏幕窗口
	//左 右 下 上 前 后
	cam->setProjectionMatrix(osg::Matrix::ortho(-1.5, 1.5, -1.5, 1.5, -10, 10));
	
	cam->setRenderOrder(osg::Camera::POST_RENDER);
	cam->setClearMask(GL_DEPTH_BUFFER_BIT);
	cam->setAllowEventFocus(false);
	cam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	cam->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	cam->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    return cam.release();
}

double getHeight(MapNode* map)
{
    double out, outm;
	map->getTerrain()->getHeight(map->getMapSRS(), 121.175, 38.736, &outm, &out);
	cout <<"MSL: " <<outm <<" elli: " <<out <<endl;
	return outm;
    //osgEarth::ElevationQuery query(map->getMap());
	//GeoPoint p(map->getMapSRS()->getGeographicSRS(), 121.175, 38.736);
    //float h = query.getElevation(p);
	//return h;
}

#ifdef PPP
<model name ="model" driver="simple">
    <url>../data/red_flag.osg.100,100,100.scale</url>
    <location>-74.018 40.717 10</location>
</model>
#endif