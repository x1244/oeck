#pragma once
#include <osgQOpenGL/osgQOpenGLWidget>
#include <QString>
class OsgWidget : public osgQOpenGLWidget
{
    Q_OBJECT
public:
OsgWidget(QWidget* parent = nullptr);
OsgWidget(osg::ArgumentParser* arguments, QWidget* parent = nullptr);
signals:
void msg(const QString& str);
protected slots:
void finishInit();
void showMessage(const QString& str);
protected:
void keyPressEvent(QKeyEvent* event) override;
private:
osg::ArgumentParser* args_;
};
