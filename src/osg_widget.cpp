#include "osg_widget.h"
#include <osgQOpenGL/OSGRenderer>
OsgWidget::OsgWidget(QWidget* parent)
  : osgQOpenGLWidget(parent)
{
}
OsgWidget::OsgWidget(osg::ArgumentParser* arguments, QWidget* parent)
  : osgQOpenGLWidget(arguments, parent)
{
}
void OsgWidget::keyPressEvent(QKeyEvent* event)
{
    m_renderer->keyPressEvent(event);
}
