#include "MouseIntersection.h"
#include <osgEarth/Controls>
#include <osg/Node>
#include <sstream>
MouseIntersection::MouseIntersection(osgViewer::Viewer* v, osg::Group* root, osgEarth::MapNode* map)
  : v_(v), root_(root), map_(map)
{
	buildLabelControls();
}

MouseIntersection::~MouseIntersection() {

}

void MouseIntersection::buildLabelControls() {
	root_->addChild(osgEarth::Controls::ControlCanvas::get(v_));
	osgEarth::Controls::ControlCanvas* canvas = osgEarth::Controls::ControlCanvas::get(v_);

	_label = new osgEarth::Controls::LabelControl;
	_label->setText("label test");
	_label->setHorizAlign(osgEarth::Util::Controls::LabelControl::Alignment::ALIGN_LEFT);
	_label->setVertAlign(osgEarth::Util::Controls::LabelControl::Alignment::ALIGN_BOTTOM);
	//_label->setEncoding(osgText::String::Encoding::ENCODING_ASCII);
	_label->setMargin(osgEarth::Util::Controls::LabelControl::Side::SIDE_BOTTOM, 10.0);
	_label->setBackColor(osg::Vec4(0, 0, 0, 0.2));
	canvas->addControl(_label);

	// 添加中文字体
	osg::ref_ptr<osgText::Text> t = new osgText::Text;
	//t->setFont("./Map/3D/fonts/simsun.ttc");
	_label->setFont(t->getFont());
	_label->setEncoding(osgText::String::Encoding::ENCODING_UTF8);
}

bool MouseIntersection::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
	if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG) {
		osgUtil::LineSegmentIntersector::Intersections intersections;
		osg::NodePath np;

		np.push_back(EarthMapMgr()->getMapNode());
		if (v_->computeIntersections(ea.getX(), ea.getY(), np, intersections)) {
			if (intersections.size() >= 1) {
				osgUtil::LineSegmentIntersector::Intersections::iterator it = intersections.begin();
				osg::Vec3d point;
				point = it->getWorldIntersectPoint();
				double latitude = 0.0;
				double longitude = 0.0;
				double height = 0.0;
				map_->getMapSRS()->getEllipsoid()
					->convertXYZToLatLongHeight(point.x(), point.y(), point.z(), latitude, longitude, height);
					
				std::string tx("xxxxxxxxx");
				_label->setText(tx);
#ifdef XXX				
				QString context = QString::fromLocal8Bit("经度:") + QString::number(osg::RadiansToDegrees(longitude)) + ", "
					+ QString::fromLocal8Bit("纬度:") + QString::number(osg::RadiansToDegrees(latitude)) + ", "
					+ QString::fromLocal8Bit("海拔:") + QString::number(height);
				_label->setText(context.toStdString());
#endif
			}
		}
	}

	return false;
}
