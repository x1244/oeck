#include "angle_site.h"
#include <iostream>
#include <osg/MatrixTransform>

using namespace std;
AngleSite::AngleSite(osg::MatrixTransform* t)
  : trans_(t), offset_(0.), add_(true)
{
}
void AngleSite::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	const double lim = 200. - 10.;
	static unsigned int ix = 10.;
	if(++ix){
		if(node == trans_){
			if(offset_ >= lim){
				add_ = false;
			}
			else if(offset_ <= 0.){
				add_ = true;
			}
			
			if(add_){
				offset_ += 2.;
			}
			else{
				offset_ -= 5.;
			}
			trans_->setMatrix(osg::Matrix::translate(1920./2, offset_, 0.));
		}
	}
}