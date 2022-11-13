/**\file
 *
 * \brief 测量设备模型
 * \author x1244 <x11244@126.com>
 */
#include <osgSim/DOFTransform>
#include <osg/Group>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
/**\brief 具备俯仰和方位角跟踪能力的设备模型
 *
 * 根据逻辑关系依次构建设备
 *  - buildBase()
 *  - buildApart()
 *  - buildEpart()
 */
class Ttp : public osg::Group
{
public:
Ttp() = default;
Ttp(const Ttp&,const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
    META_Node(osg, Ttp);
/**\brief 构建设备底座
 *
 * \param base - 底座节点
 */
void buildBase(osg::Node* base);
/**\brief 构建方位跟踪部件
 *
 * \param a - 方位节点
 * \param offset - 相对底座的坐标
 */
void buildApart(osg::Node* a, const osg::Vec3& offset);
/**\brief 构建俯仰跟踪部件
 *
 * \param e - 俯仰节点
 * \param offset - 相对方位节点的坐标
 */
void buildEpart(osg::Node* e, const osg::Vec3& offset);
/**\brief 更新AE状态
 *
 * \param a - 方位角
 * \param e - 俯仰角
 */
void updateAe(double a, double e);
/**\brief 底座节点
 *
 * \return osg:Node* - 底座节点
 */
osg::Node* base() const;
/**\brief 方位节点
 *
 * \param osg::Node* - 方位节点
 */
osg::Node* apart() const;
/**\brief 俯仰节点
 *
 * \param osg::Node* - 俯仰节点
 */
osg::Node* epart() const;
private:
using Base = osg::observer_ptr<osg::Node>;
using Doft = osg::observer_ptr<osgSim::DOFTransform>;
Base base_;                            /**< 底座 */
Doft apart_;                           /**< 方位部件 */
Doft epart_;                           /**< 俯仰部件 */
};
