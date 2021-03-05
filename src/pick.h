#pragma once

#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>

class PickHandler :  public osgGA::GUIEventHandler
{
public:
PickHandler(osgViewer::Viewer *viewer);
~PickHandler();
bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
protected:
void pick(float fX, float fY);
private:
osgViewer::Viewer *viewer_;

};
