/**\file 
 *
 * \brief 风场精灵节点
 * \author x1244 <x11244@126.com>
 */
#pragma once
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture>
namespace ick{
class SpritePass1;                     /**< 第一遍渲染精灵 */
class SpritePass2;                     /**< 第二遍渲染精灵 */
/**\brief 风场精灵
 *
 * 该场以一个Geode节点包含两个Geometry元素，以Geometry来实际绘制精灵。
 * 两个Geometry以TransformFeedback环路来交换精灵位置缓冲区，
 * 在GPU端更新数据，避免了昂贵的CPU开销。
 * 第一个Geometry更新精灵位置和风速大小，
 * 第二个Geometry用来绘制。
 *
 * 在使用WindSprite时，请不要给它再添加其他的Geometry，
 * 也不要删除它自身的Geometry。
 */
class WindSprite: public osg::Geode{
public:
WindSprite();
/**\brief 拷贝构造
 *
 * 目前拷贝构造什么也不做。
 * 希望在接下来：
 * ------------->只是浅拷贝什么都不做。
 * ------------->深拷贝会原地重建。
 */
WindSprite(const WindSprite&, const osg::CopyOp& cp=osg::CopyOp::SHALLOW_COPY);
    META_Node(osg, WindSprite);
/**\brief 设置为平面模式
 */
void setPlateMode();
/**\brief 设置为球模式
 */
void setSphereMode();
/**\brief 应用风场uv纹理
 *
 * \param uv - 风场uv纹理
 * \param mini - 最小风速
 * \param maxi - 最大风速
 * \param h - 分层高度
 */
void applyWind(osg::ref_ptr<osg::Texture> uv, float mini, float maxi, float h);
/**\brief 召唤风精灵
 *
 * \param count - 精灵数量
 * \param pt - 精灵点大小
 */
void callSprites(int count, float pt);
protected:
/**\brief 创建风精灵
 */
void createSprites();
/**\brief 构建风精灵
 *
 * \param g - 精灵子
 * \param count - 精灵数
 */
void makeSprites(osg::Geometry* g, int count) const;
private:
SpritePass1* sprite1_;                 /**< 精灵一 */
SpritePass2* sprite2_;                 /**< 精灵二 */
};//class WindSprite
//---------------------------------------------------------
//                    class SpritePass1
//---------------------------------------------------------
class SpritePass1 : public osg::Geometry
{
public:
SpritePass1();
SpritePass1(const SpritePass1&
    , const osg::CopyOp& cp = osg::CopyOp::SHALLOW_COPY);
    META_Node(osg, SpritePass1);
/**\brief 反馈目标
 *
 * \param to - 反馈目标
 */
void feedbackTo(osg::Geometry* to);
/**\brief 应用着色器
 */
void applyShader();
protected:
void drawImplementation(osg::RenderInfo& renderInfo) const override;
private:
osg::VertexBufferObject* vbo_;         /**< 反馈VBO */
};//class SpritePass1
//---------------------------------------------------------
//                    class SpritePass2
//---------------------------------------------------------
class SpritePass2 : public osg::Geometry
{
public:
SpritePass2();
SpritePass2(const SpritePass2&
    , const osg::CopyOp& cp = osg::CopyOp::SHALLOW_COPY);
    META_Node(osg, SpritePass2);
/**\brief 反馈目标
 *
 * \param to - 反馈目标
 */
void feedbackTo(osg::Geometry* to);
/**\brief 应用着色器
 */
void applyShader();
/**\brief 设置精灵点大小
 *
 * \param pt - 点大小
 */
void setPointSize(float pt);
/**\brief 设置为平面模式
 */
void setPlateMode();
/**\brief 设置为球模式
 */
void setSphereMode();
protected:
void drawImplementation(osg::RenderInfo& renderInfo) const override;
private:
osg::VertexBufferObject* vbo_;         /**< 反馈VBO */
bool sphere_;                          /**< 球模式 */
float pt_;                             /**< 精灵点大小 */
};//class SpritePass2
}//namespace ick
