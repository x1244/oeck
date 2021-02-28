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
#include <osgText/Text>
#include <osgViewer/Viewer>
#include <osgEarth/Controls>
#include <osgViewer/ViewerEventHandlers>

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
osg::Camera* createBackground(std::string strImg);
osg::Camera* createForeground(std::string strImg);
osg::MatrixTransform* createHud(std::string str);
osg::Camera* hudCamera();

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
osg::Camera* createForeground(std::string strImg)
{
	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry1 = new osg::Geometry;
    osg::ref_ptr<osg::Camera> camera1 = new osg::Camera;

    camera1->setAllowEventFocus(false);
    camera1->setProjectionMatrixAsOrtho2D(0, 1920, 0, 1280);
    //camera1->setViewport(0, 0, 960, 600);

    camera1->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera1->setRenderOrder(osg::Camera::POST_RENDER);
    camera1->setClearMask(GL_DEPTH_BUFFER_BIT);
    //camera1->setClearColor(osg::Vec4());
    camera1->setViewMatrix(osg::Matrix::identity());

    //压入顶点
    osg::ref_ptr<osg::Vec3Array> vertex = new osg::Vec3Array;
    vertex->push_back(osg::Vec3(0.0,0.0,0.0));
    vertex->push_back(osg::Vec3(1920.0, 0.0, 0.0));
    vertex->push_back(osg::Vec3(1920.0, 200.0, 0.0));
    vertex->push_back(osg::Vec3(0.0, 200.0, 0.0));
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
        std::cout <<"load image " <<strImg <<" error" << std::endl;
    }

    osg::ref_ptr<osg::Texture2D> texture2d = new osg::Texture2D;
    texture2d->setImage(0, img1);
    geometry1->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture2d,osg::StateAttribute::ON);


    camera1->addChild(geode1);
    geode1->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    geode1->addDrawable(geometry1);

    return camera1.release();
}
osg::MatrixTransform* createHud(std::string str)
{
	double r = 1.5, h = -1.0;
	osg::Vec3 corner(-r, -r, h);
	osg::Vec3 w(2*r, 0., h);
	osg::Vec3 v(0., 2*r, 0.);
	osg::ref_ptr<osg::Drawable> quad = createTexturedQuadGeometry(corner, w, v);
	
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage(osgDB::readImageFile(str));
	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
	//quad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	//quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	
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
