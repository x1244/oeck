#pragma once
#include <osg/Callback>
namespace osgEarth{namespace Util{namespace Controls{
class ImageControl;
}}}
class Panorama : public osg::NodeCallback
{
public:
explicit Panorama(osgEarth::Util::Controls::ImageControl* ctrl);
void operator()(osg::Node* node, osg::NodeVisitor* nv) override;
private:
osgEarth::Util::Controls::ImageControl* ctrl_;
osg::ref_ptr<osg::Image> m[2];
};