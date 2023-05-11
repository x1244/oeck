#include "wind_shader.h"
#include <osg/Geometry>
using namespace osg;
using namespace std;
namespace ick{
void applyShader(StateSet* ss, const string& vert, const string& frag)
{
    osg::Shader* vShader = new osg::Shader(osg::Shader::VERTEX, vert);
    osg::Shader* fShader = new osg::Shader(osg::Shader::FRAGMENT, frag);
    osg::Program* program = new osg::Program;
    program->addShader(vShader);
    program->addShader(fShader);
	ss->setAttributeAndModes(program, osg::StateAttribute::ON);
}
ref_ptr<Geode> createQuadWithTexture(float w, float h, osg::Texture* tex)
{
    using namespace osg;
	osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array;
	verts->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	verts->push_back(osg::Vec3(w, 0.0f, 0.0f));
	verts->push_back(osg::Vec3(w, 0.0f, h));
	verts->push_back(osg::Vec3(0.0f, 0.0f, h));
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
	texcoords->push_back(osg::Vec2(0.0f, 0.0f));
	texcoords->push_back(osg::Vec2(1.0f, 0.0f));
	texcoords->push_back(osg::Vec2(1.0f, 1.0f));
	texcoords->push_back(osg::Vec2(0.0f, 1.0f));
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::TRIANGLE_FAN, 0, 4));
	geom->setVertexArray(verts);
	geom->setVertexAttribArray(1, texcoords);
	geom->setVertexAttribBinding(1, Geometry::BIND_PER_VERTEX);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom);
	auto ss = geode->getOrCreateStateSet();
	ss->setTextureAttributeAndModes(0, tex);
	applyShader(ss, simple_vert, simple_frag);
    return geode;
}
}//namespace ick
