#include "info_visitor.h"
#include <iostream>
#include <icecream.hpp>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/StateSet>
using namespace std;
void InfoVisitor::apply(osg::Node& node)
{
    std::cout << space() <<"node:" <<&node <<endl;
	std::cout << space() << node.libraryName() << "::"
		<< node.className() << std::endl;
	showAttribute(node);

    osg::Geometry* g = dynamic_cast<osg::Geometry*>(&node);
    if(g){
        int c = g->getNumTexCoordArrays();
        cout <<space() <<"  **** texture arrays:" <<c;
        if(c > 0){
            osg::Array* tc = g->getTexCoordArray(0);
            cout << space() <<"  **** texture array [0] size:"<<tc->getNumElements() <<endl;
        }
    }

	auto ss = node.getOrCreateStateSet();
#if 1
    auto m = ss->getAttribute(osg::StateAttribute::MATERIAL, 0);
    std::cout <<space() <<" --" <<m <<endl;
//    if(m) ss->setAttributeAndModes(m, osg::StateAttribute::OFF);
    if(m) ss->removeAttribute(osg::StateAttribute::MATERIAL, 0);
#endif

    int ctx = ss->getNumTextureAttributeLists();
    if(ctx){
        std::cout <<space() <<" textures:" <<ctx <<endl;
        for(int ix = 0; ix < ctx; ++ix){
            auto tx = ss->getTextureAttribute(ix, osg::StateAttribute::TEXTURE);
            cout <<space() <<"   ##texture:" <<tx <<endl;
        }
    }

	_level++;
	traverse(node);
	_level--;
}

void InfoVisitor::apply(osg::Geode& geode)
{
    std::cout << space() <<"geode:" <<&geode <<endl;
	std::cout << space() << geode.libraryName() << "::"
		<< geode.className() << std::endl;

	showAttribute(geode);
#if 1
	auto ss = geode.getOrCreateStateSet();
    auto m = ss->getAttribute(osg::StateAttribute::MATERIAL, 0);
    std::cout <<space() <<" --" <<m <<endl;
//    if(m) ss->setAttributeAndModes(m, osg::StateAttribute::OFF);
    if(m) ss->removeAttribute(osg::StateAttribute::MATERIAL, 0);

    auto tx = ss->getAttribute(osg::StateAttribute::TEXTURE, 0);
//    IC(tx);
#endif

	_level++;
    std::cout <<space() <<"drawables: " <<geode.getNumDrawables() <<endl;
#if 0
	for (unsigned int i = 0; i < geode.getNumDrawables(); i++)
	{
		osg::Drawable* drawable = geode.getDrawable(i);
		std::cout << space() << drawable->libraryName() << "::"
			<< drawable->className() << std::endl;
        std::cout <<space() <<" ==" <<drawable <<endl;
		showAttribute(*drawable);
	}
#endif
	traverse(geode);
	_level--;
}
void InfoVisitor::showAttribute(osg::Node& node)
{
    cout <<space() <<"attributes:" <<&node <<endl;
    auto ss = node.getStateSet();
    if(!ss){
        cout <<space() <<"  no state attribute" <<endl;
        return ;
    }
    auto& attrs = ss->getAttributeList();
    if(attrs.empty()){
        cout <<space() <<"  empty attribute list" <<endl;
        return ;
    }
    for(auto& attr : attrs){
        auto& key = attr.first;
        cout <<space() <<" +TYPE:" <<key.first <<" index:" <<key.second <<endl;
        auto p = attr.second.first;
        cout <<space() <<"  " <<p.get() <<endl;
    }
}
