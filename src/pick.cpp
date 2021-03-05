#include "pick.h"
#include <iostream>
using namespace std;
#include <osgUtil/LineSegmentIntersector>
#include <osgEarth/PrimitiveIntersector>

PickHandler::PickHandler(osgViewer::Viewer *viewer)
  :viewer_(viewer)
{
}
PickHandler::~PickHandler()
{
}
bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    switch (ea.getEventType())
    {
     case(osgGA::GUIEventAdapter::PUSH):
        {
            if (ea.getButton() == 1)
            {
                pick(ea.getX(),ea.getY());
            }
            return false;
        }
    }
    return false;
}
void PickHandler::pick(float fX, float fY )
{
    osgUtil::LineSegmentIntersector::Intersections intersections;
    if (viewer_->computeIntersections(fX,fY,intersections))
    {
        for (auto itr = intersections.begin(); itr != intersections.end(); ++itr)
        {
            if (!itr->nodePath.empty())
            {
                const osg::NodePath& np = itr->nodePath;
                for (int i = np.size() - 1; i >= 0; --i)
                {
                    osg::ref_ptr<osg::Node> node = dynamic_cast<osg::Node *>(np[i]);
                    if(node)
                    {
						cout <<"pick " <<node->getName() <<endl;
                        //node->setNodeMask(0);
                        //itr->getWorldIntersectPoint(); // 得到坐标
                    }
                }
            }
        }
    }
}
