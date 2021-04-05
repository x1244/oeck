#include <osgQOpenGL/osgQOpenGLWidget>
class OsgWidget : public osgQOpenGLWidget
{
    Q_OBJECT
public:
OsgWidget(QWidget* parent = nullptr);
OsgWidget(osg::ArgumentParser* arguments, QWidget* parent = nullptr);
protected:
void keyPressEvent(QKeyEvent* event) override;
};
