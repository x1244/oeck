#include "panorama.h"
#include <iostream>
#include <osg/Texture>
#include <osg/ref_ptr>
#include <osg/Image>
#include <osgDB/ReadFile>
#include <osgEarth/Controls>


using namespace std;
Panorama::Panorama(osgEarth::Util::Controls::ImageControl* ctrl)
  : ctrl_(ctrl)
{
	m[0] = osgDB::readImageFile("../data/1.png");
	m[1] = osgDB::readImageFile("../data/2.png");
}
void Panorama::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	static unsigned int ix = 0;
	if(node == ctrl_){
		int md = ++ix%60;
	    if(md == 0){
            ctrl_->setImage(m[0].get());
        }
		else if(md == 30){  
		    ctrl_->setImage(m[1].get());
		}
	}
}