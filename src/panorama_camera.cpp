#include "panorama_camera.h"
#include <iostream>
#include <osg/MatrixTransform>

using namespace std;
PanoramaCamera::PanoramaCamera(osg::MatrixTransform* t)
  : trans_(t), offset_(0.), add_(true)
{
}
void PanoramaCamera::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	const double lim = 1920/2.;
	static unsigned int ix = 0;
	if(++ix){
		if(node == trans_){
			if(offset_ >= lim){
				add_ = false;
			}
			else if(offset_ <= -lim){
				add_ = true;
			}
			
			if(add_){
				offset_ += 5;
			}
			else{
				offset_ -= 10;
			}
			trans_->setMatrix(osg::Matrix::translate(offset_,0,0));
		}
	}
}