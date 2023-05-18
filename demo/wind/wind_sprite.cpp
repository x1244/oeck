#include "wind_sprite.h"
#include <icecream.hpp>
#include "wind_shader.h"

/**\brief 设置float变量
 *
 * \param ss - 状态机
 * \param name - 变量名
 * \param v - 值
 */
static void setUniformFloat(osg::StateSet* ss, const std::string& name, float v)
{
    using namespace osg;
    if(UniformBase* base = ss->getUniformBase(name)){
        static_cast<osg::TemplateUniform<float>*>(base)->setValue(v);
    }
    else{
        osg::ref_ptr<osg::TemplateUniform<float> >u 
            = new osg::TemplateUniform<float>(name, v);
        ss->addUniform(u);
    }
}
//---------------------------------------------------------
//                   class WindSprite
//---------------------------------------------------------
namespace ick{

WindSprite::WindSprite()
{
    createSprites();
}
WindSprite::WindSprite(const WindSprite& other, const osg::CopyOp& cp)
{
    //TODO
}
void WindSprite::applyWind(osg::ref_ptr<osg::Texture> uv
                            , float mini, float maxi, float h)
{
    auto ss1 = sprite1_->getOrCreateStateSet();
    ss1->setTextureAttributeAndModes(0, uv);
    setUniformFloat(ss1, "mini", mini);
    setUniformFloat(ss1, "mpx", maxi - mini);
    auto ss2 = sprite2_->getOrCreateStateSet();
    setUniformFloat(ss2, "h", h);
}
void WindSprite::setPlateMode()
{
    sprite2_->setPlateMode();
}
void WindSprite::setSphereMode()
{
    sprite2_->setSphereMode();
}
void WindSprite::callSprites(int count, float pt)
{
    makeSprites(sprite1_, count);
    makeSprites(sprite2_, count);
    sprite1_->applyShader();
    sprite2_->applyShader();
    sprite1_->feedbackTo(sprite2_);
    sprite2_->feedbackTo(sprite1_);
    sprite2_->setPointSize(pt);
}
void WindSprite::createSprites()
{
    sprite1_ = new SpritePass1;
    addDrawable(sprite1_);

    sprite2_ = new SpritePass2;
    addDrawable(sprite2_);
}
void WindSprite::makeSprites(osg::Geometry* g, int count) const
{
    //清空
    g->removePrimitiveSet(0, g->getNumPrimitiveSets());
    osg::Array* va = g->getVertexArray();
    if(va){
        va->resizeArray(count);
        va->dirty();
    }
    else{
        va = new osg::Vec4Array(count);
        g->setVertexArray(va);
    }
    g->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, count));
}
//---------------------------------------------------------
//                       class SpritePass1
//---------------------------------------------------------
SpritePass1::SpritePass1()
{
    setUseVertexBufferObjects(true);
}
SpritePass1::SpritePass1(const SpritePass1&, const osg::CopyOp& cp)
{
    //TODO
}
void SpritePass1::feedbackTo(osg::Geometry* to)
{
    vbo_ = to->getOrCreateVertexBufferObject();
}
void SpritePass1::drawImplementation(osg::RenderInfo& renderInfo) const
{
    unsigned int contextID = renderInfo.getState()->getContextID();
    GLuint ubuff = vbo_->getOrCreateGLBufferObject(contextID)->getGLObjectID();
    osg::GLExtensions* ext = renderInfo.getState()->get<osg::GLExtensions>();
    ext->glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ubuff);

    glEnable(GL_RASTERIZER_DISCARD);
    ext->glBeginTransformFeedback(GL_POINTS);
    osg::Geometry::drawImplementation(renderInfo);
    ext->glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);
    ext->glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
}
void SpritePass1::applyShader()
{
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, sprite_pass_1_vert);
    osg::Program* program = new osg::Program;
    program->addShader(vShader);
    program->setTransformFeedBackMode(GL_INTERLEAVED_ATTRIBS);
    auto ss = getOrCreateStateSet();
    ss->setAttributeAndModes(program, osg::StateAttribute::ON);
}
//---------------------------------------------------------
//                       class SpritePass2
//---------------------------------------------------------
SpritePass2::SpritePass2()
  : sphere_(true)
{
    setUseVertexBufferObjects(true);
    setSphereMode();
}
SpritePass2::SpritePass2(const SpritePass2& other, const osg::CopyOp& cp)
  : sphere_(other.sphere_)
{
    //TODO
    setSphereMode();
}
void SpritePass2::feedbackTo(osg::Geometry* to)
{
    vbo_ = to->getOrCreateVertexBufferObject();
}
void SpritePass2::setPointSize(float pt)
{
    auto ss = getOrCreateStateSet();
	ss->setMode(GL_VERTEX_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
    setUniformFloat(ss, "pt", pt);
}
void SpritePass2::drawImplementation(osg::RenderInfo& renderInfo) const
{
    unsigned int contextID = renderInfo.getState()->getContextID();
    GLuint ubuff= vbo_->getOrCreateGLBufferObject(contextID)->getGLObjectID();
    osg::GLExtensions* ext = renderInfo.getState()->get<osg::GLExtensions>();
    ext->glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ubuff);

    ext->glBeginTransformFeedback(GL_POINTS);
    osg::Geometry::drawImplementation(renderInfo);
    ext->glEndTransformFeedback();
    ext->glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
}
void SpritePass2::applyShader()
{
    osg::Shader* vShader;
    if(sphere_){
        vShader = new osg::Shader(osg::Shader::VERTEX, sprite_pass_2_vert);
    }
    else{
        vShader = new osg::Shader(osg::Shader::VERTEX, sprite_pass_0_vert);
    }
    osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT, wind_sprite_2_frag);
    osg::Program* program = new osg::Program;
    program->addShader(vShader);
    program->addShader(fShader);
    program->setTransformFeedBackMode(GL_INTERLEAVED_ATTRIBS);
    auto ss = getOrCreateStateSet();
    ss->setAttributeAndModes(program, osg::StateAttribute::ON);
}
void SpritePass2::setPlateMode()
{
    sphere_ = false;
    osg::BoundingBox bb(0.0, 0.0, 0.0, 1.0, 0.0, 0.5);
    setInitialBound(bb);
}
void SpritePass2::setSphereMode()
{
    sphere_ = true;
    osg::BoundingBox bb(-6375000.0, -6375000.0, -6375000.0,
            6375000.0, 6375000.0, 6375000.0);
    setInitialBound(bb);
}

}//namespace ick
