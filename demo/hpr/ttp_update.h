/**\file
 *
 * \brief 跟踪参数更新回调
 * \author x1244 <x11244@126.com>
 */
#include <osg/Callback>
/**\brief 设备跟踪参数更新回调
 */
class TtpUpdate : public osg::Callback
{
public:
bool run(osg::Object* object, osg::Object* data) override;
};
