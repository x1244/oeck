#pragma once
#include <osg/Callback>
namespace osg{
class MatrixTransform;
}
class PanoramaCamera : public osg::NodeCallback
{
public:
explicit PanoramaCamera(osg::MatrixTransform* t);
void operator()(osg::Node* node, osg::NodeVisitor* nv) override;
private:
osg::MatrixTransform* trans_;
double offset_;
bool add_;
};