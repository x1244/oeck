#include "ttp.h"
#include <icecream.hpp>
using namespace osg;
Ttp::Ttp(const Ttp& other, const CopyOp& copyop)
  : osg::Group(other, copyop)
{
    if(other.base_.valid()){
        auto base = static_cast<Node*>(other.base_->clone(copyop));
        auto a = static_cast<Node*>(other.apart_->getChild(0)->clone(copyop));
        auto e = static_cast<Node*>(other.epart_->getChild(0)->clone(copyop));
        buildBase(base);
        buildApart(a, other.apart_->getCurrentTranslate());
        buildEpart(e, other.epart_->getCurrentTranslate());
    }
}
void Ttp::buildBase(osg::Node* base)
{
    if(base_.valid()) removeChild(base_.get());
    base_ = base;
    addChild(base);
}
void Ttp::buildApart(osg::Node* a, const osg::Vec3& offset)
{
    apart_ = new osgSim::DOFTransform;
    apart_->addChild(a);
    //这里一定要设置scale,因为默认是0,导致无显示
    apart_->setCurrentScale(osg::Vec3(1., 1., 1.));
    apart_->setCurrentTranslate(offset);
    addChild(apart_.get());
}
void Ttp::buildEpart(osg::Node* e, const osg::Vec3& offset)
{
    epart_ = new osgSim::DOFTransform;
    epart_->addChild(e);
    //这里一定要设置scale,因为默认是0,导致无显示
    epart_->setCurrentScale(osg::Vec3(1., 1., 1.));
    epart_->setCurrentTranslate(offset);
    apart_.get()->addChild(epart_.get());
}
void Ttp::updateAe(double a, double e)
{
    if(apart_.valid()){
        //对应于方
        Vec3 z(DegreesToRadians(a), 0., 0.);
        apart_->setCurrentHPR(z);
    }
    if(epart_.valid()){
        //对应于滚转
        Vec3 y(0., 0., DegreesToRadians(e));
        epart_->setCurrentHPR(y);
    }
}
osg::Node* Ttp::base() const
{
    return base_.get();
}
osg::Node* Ttp::apart() const
{
    return apart_.valid() ? apart_.get()->getChild(0) : nullptr;
}
osg::Node* Ttp::epart() const
{
    return epart_.valid() ? epart_.get()->getChild(0) : nullptr;
}
