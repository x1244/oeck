#include "wind_scene.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Program>
#include <osg/Shader>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TextureBuffer>
#include <osg/Uniform>
#include <icecream.hpp>
#include "wind_shader.h"
using namespace osg;
using namespace ick;
namespace{
const int widths  = 2048;              /**< 风精灵纹理宽度 */
const int heights = 1024;              /**< 风精灵纹理高度 */
}
/**\brief 设置float变量
 *
 * \param ss - 状态机
 * \param name - 变量名
 * \param v - 值
 */
static void setUniformFloat(StateSet* ss, const std::string& name, float v)
{
    if(UniformBase* base = ss->getUniformBase(name)){
        static_cast<osg::TemplateUniform<float>*>(base)->setValue(v);
    }
    else{
        osg::ref_ptr<osg::TemplateUniform<float> >u 
            = new osg::TemplateUniform<float>(name, v);
        ss->addUniform(u);
    }
}
/**\brief 设置int变量
 *
 * \param ss - 状态机
 * \param name - 变量名
 * \param v - 值
 */
static void setUniformInt(StateSet* ss, const std::string& name, int v)
{
    if(UniformBase* base = ss->getUniformBase(name)){
        static_cast<osg::TemplateUniform<int>*>(base)->setValue(v);
    }
    else{
        osg::ref_ptr<osg::TemplateUniform<int> >u 
            = new osg::TemplateUniform<int>(name, v);
        ss->addUniform(u);
    }
}

void WindScene::createCanvas(int points, float pt, bool plate)
{
    sprites_ = points;
    pt_ = pt;
    plate_ = plate;
    pt1_ = createTextureFloat(widths, heights);
    pt2_ = createTextureFloat(widths, heights);
    test_ = createTextureFloat(widths, heights);
    createWind(points, pt);
}
osg::ref_ptr<osg::Texture> WindScene::testTex()
{
    return test_;
}
osg::ref_ptr<osg::Texture> WindScene::createTexture2D(int w, int h) const
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	//可以不用设置DYNAMIC，
//    texture->setDataVariance(Object::DYNAMIC);
#if 0
    osg::ref_ptr<osg::Image> img = new osg::Image;
    img->allocateImage(w, h, 1, GL_FLOAT, GL_RGBA32F);
    texture->setImage(img.get());
#endif
	texture->setTextureSize(w, h);
	texture->setInternalFormat(GL_RGBA);
	texture->setSourceFormat(GL_RGBA);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	return texture;
}
osg::ref_ptr<osg::Texture> WindScene::createTextureFloat(int w, int h) const
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setTextureSize(w, h);
	texture->setInternalFormat(GL_RGBA32F);
	texture->setSourceFormat(GL_RGBA);
	texture->setSourceType(GL_FLOAT);
	texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	return texture;
}
osg::ref_ptr<osg::Camera> WindScene::createRttCamera(int p, int w, int h) const
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	camera->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::PRE_RENDER, p);
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    camera->setAllowEventFocus(false);
    //left right bottom top
    camera->setProjectionMatrixAsOrtho2D(0, 1.0, 0, 1.0); 
    //x y with heght
    camera->setViewport(0, 0, w, h);
    //camera->setViewMatrix(osg::Matrix::identity());
	osg::Vec3 eye(0., -1.0, 0.);
    osg::Vec3 center(0.0, 0.0, 0.0);
    osg::Vec3 up(0.0, 0.0, 1.0);
	camera->setViewMatrixAsLookAt(eye, center, up);
	return camera;
}
void WindScene::applyParent(osg::Group* parent)
{
    parent_ = parent;
    parent_->addChild(pc1_);
    parent_->addChild(pc2_);
    if(plate_){//to [0, 1] & [0, 0.5]
        MatrixTransform* mxt = new MatrixTransform;
        Matrix mx;
        mx.makeScale(Vec3d(1.0, 1.0, 0.5));
        mxt->setMatrix(mx);
        mxt->addChild(sprite0_);
        parent_->addChild(mxt);
    }
    else{
        parent->addChild(sprite0_);
    }
}
void WindScene::setWindUv(ref_ptr<Texture> uv, float mini, float maxi, float h)
{
    uv_ = uv;
    mini_ = mini;
    maxi_ = maxi;
    h_ = h;
}
void WindScene::createWind(int points, float pt)
{
    assert(points < widths*heights);
    sprites_ = points;
    pt_ = pt;
    sprite1_ = createSprites(points, pt);
    auto ss1 = sprite1_->getOrCreateStateSet();
    applyShader(ss1, sprite_1_vert, sprite_1_frag);
    ss1->setTextureAttributeAndModes(0, uv_);
    ss1->setTextureAttributeAndModes(1, pt1_);
    setUniformFloat(ss1, "widths", float(widths));
    setUniformFloat(ss1, "pt", pt_);
    setUniformFloat(ss1, "mini", mini_);
    setUniformFloat(ss1, "mpx", maxi_ - mini_);
    pc1_ = createRttCamera(0, widths, heights);
    pc1_->attach(Camera::COLOR_BUFFER0, pt2_);
    pc1_->attach(Camera::COLOR_BUFFER1, test_);
    pc1_->addChild(sprite1_);


    sprite2_ = createSprites(points, pt);
    auto ss2 = sprite2_->getOrCreateStateSet();
    ss2->setTextureAttributeAndModes(0, pt2_);
    setUniformFloat(ss2, "widths", float(widths));
    setUniformFloat(ss2, "pt", pt_);
    applyShader(ss2, sprite_2_vert, sprite_2_frag);

    pc2_ = createRttCamera(1, widths, heights);
    pc2_->attach(Camera::COLOR_BUFFER0, pt1_);
//    pc2_->attach(Camera::COLOR_BUFFER1, test_);
    pc2_->addChild(sprite2_);

    sprite0_ = createSprites(points, pt);
    auto ss0 = sprite0_->getOrCreateStateSet();
    ss0->setTextureAttributeAndModes(0, pt2_);
    setUniformFloat(ss0, "widths", float(widths));
    setUniformFloat(ss0, "pt", pt_);
    setUniformFloat(ss0, "h", h_);
    if(plate_){
        applyShader(ss0, wind_1_vert, wind_1_frag);
    }
    else{
        applyShader(ss0, wind_2_vert, wind_1_frag);
    }
}
ref_ptr<Geode> WindScene::createSprites(int points, float pt) const
{
    using namespace osg;
    ref_ptr<Geode> geode = new Geode;
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
    verts->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::POINTS, 0, 1, points));
//    osg::ref_ptr<osg::PrimitiveSet> pr 
//        = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, 1);
//    pr->setNumInstances(count);
//    geom->addPrimitiveSet(pr);
	geom->setVertexArray(verts);
	if(plate_){
        osg::BoundingBox bb(0.0, 0.0, 0.0, 1.0, 0.0, 1.0);
        geom->setInitialBound(bb);
    }
    else{
        osg::BoundingBox bb(-6375000.0, -6375000.0, -6375000.0,
            6375000.0, 6375000.0, 6375000.0);
        geom->setInitialBound(bb);
    }

	geode->addDrawable(geom);
	auto ss = geode->getOrCreateStateSet();
	//由GLSL控制点大小
	ss->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
	ss->addUniform(new osg::TemplateUniform<float>("pt", pt));
    return geode;
}
