#include "ttp_update.h"
#include <cmath>
#include <osg/FrameStamp>
#include <icecream.hpp>
#include "ttp.h"
using namespace osg;
bool TtpUpdate::run(osg::Object* object, osg::Object* data)
{
    if(Ttp* ttp = dynamic_cast<Ttp*>(object)){
        double a = 0., e = 0.;
        if(NodeVisitor* nv = dynamic_cast<NodeVisitor*>(data)){
            double t = nv->getFrameStamp()->getSimulationTime();
            double sst = sin(t);
            a = 2.*RadiansToDegrees(std::asin(sst) + 3.14/2.);
            e = RadiansToDegrees(std::acos(sst))/2.;
//            IC(a, e);
        }
        ttp->updateAe(a, e);
    }
    return true;
}