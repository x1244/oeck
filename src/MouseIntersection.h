#ifndef MOUSEINTERSECTION_H
#define MOUSEINTERSECTION_H

#include <osgGA/GUIEventHandler>
#include <osgEarth/Controls>
namespace osgEarth{
class MapNode;
}
namespace osgViewer{
class Viewer;
}
namespace osg{
class Group;
}
//经纬度标牌显示

class MouseIntersection : public osgGA::GUIEventHandler {
public:
	MouseIntersection(osgViewer::Viewer* v, osg::Group* root, osgEarth::MapNode* map);
	virtual ~MouseIntersection();

	osgEarth::Util::Controls::LabelControl*getLabel() {
		return _label;
	}
private:
	void buildLabelControls();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)override;
private:
	osgEarth::Util::Controls::LabelControl* _label;
	osgViewer::Viewer* v_;
	osg::Group* root_;
	osgEarth::MapNode* map_;
};

#endif // MOUSEINTERSECTION_H
