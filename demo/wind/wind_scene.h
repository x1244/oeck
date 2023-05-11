/**\file 风场
 *
 * \brief 动态风场创建和更新
 * \author x1244 <x11244@126.com>
 */
#pragma once
#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Image>
#include <osg/Texture>
#include <osgViewer/Viewer>
namespace ick{
/**\brief 风场
 *
 * 风场的创建、更新。
 *
 * 利用一对相机来实现纹理缓冲区交换，
 * 相机一是主力相机，完成更新和输出，
 * 相机二是交换相机，用来交换前序缓冲区。
 *
 * 将TextureBuffer attach到相机时,不能通过，
 * 为此只能用Texture2D;
 */
class WindScene{
public:
/**\brief 创建风场
 *
 * 精灵点数不能超过2048*1024，也就是2百万吧
 *
 * \param points - 精灵点数
 * \param pt - 点大小
 * \param plate - 平面投影
 */
void createCanvas(int points, float pt, bool plate);
/**\brief 获得测试纹理
 *
 * \return osg::ref_ptr<osg::Texture> - 测试纹理
 */
osg::ref_ptr<osg::Texture> testTex();
/**\brief 应用父节点
 *
 * \param parent - 父节点
 */
void applyParent(osg::Group* parent);
/**\brief 设置风场UV分量纹理
 *
 * \param uv - uv纹理
 * \param mini - 风速最小值，一般为负风量，m/s
 * \param maxi - 风量最大值，一般为正风量，m/s
 * \param h - 风场高度，m
 */
void setWindUv(osg::ref_ptr<osg::Texture> uv, float mini, float maxi, float h);
protected:
/**\brief 创建纹理
 *
 * \param w - 纹理宽
 * \param h - 纹理高
 * \return osg::ref_ptr<osg::Texture> - 纹理
 */
osg::ref_ptr<osg::Texture> createTexture2D(int w, int h) const;
/**\brief 创建方形纹理
 *
 * \param w - 纹理宽
 * \param h - 纹理高
 * \return osg::ref_ptr<osg::Texture> - 纹理
 */
osg::ref_ptr<osg::Texture> createTextureFloat(int w, int h) const;
/**\brief 创建RTT相机
 *
 * \param p - 相机渲染顺序
 * \param w - 视场宽
 * \param h - 视场高
 * \return osg::ref_ptr<osg::Camera> - 相机
 */
osg::ref_ptr<osg::Camera> createRttCamera(int p, int w, int h) const;
/**\brief 创建风场点精灵
 *
 * \param points - 点数
 * \param pt - 点大小
 */
osg::ref_ptr<osg::Geode> createSprites(int points, float pt) const;
/**\brief 创建风场
 *
 * \param points - 精灵点数
 * \param pt - 点大小
 */
void createWind(int points, float pt);
private:
osg::Group* parent_;                   /**< 父节点 */
osg::ref_ptr<osg::Geode> sprite1_;     /**< 风场节点一 */
osg::ref_ptr<osg::Geode> sprite2_;     /**< 风场节点二 */
osg::ref_ptr<osg::Geode> sprite0_;     /**< 风场精灵 */
osg::ref_ptr<osg::Texture> uv_;        /**< 风场矢量纹理 */
osg::ref_ptr<osg::Texture> pt1_;       /**< 点纹理一 */
osg::ref_ptr<osg::Texture> pt2_;       /**< 点纹理二 */
osg::ref_ptr<osg::Camera> pc1_;        /**< 点相机1 */
osg::ref_ptr<osg::Camera> pc2_;        /**< 点相机2 */
osg::ref_ptr<osg::Texture> test_;      /**< 测试纹理 */
int sprites_;                          /**< 精灵点数量 */
float pt_;                             /**< 点大小 */
float mini_;                           /**< 最小风速 */
float maxi_;                           /**< 最大风速 */
float h_;                              /**< 风场高度 */
bool plate_{false};                    /**< 平面显示 */
};//class WindScene
}//namespace ick

