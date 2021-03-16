#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/Viewer>


using namespace std;
osg::ref_ptr<osg::Node> createSceneGraph()
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	//顶点数组
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	geom->setVertexArray(v.get());
	v->push_back(osg::Vec3(-1.0f, 0.0f, -1.0f));
	v->push_back(osg::Vec3(-1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, -1.0f));
	//颜色数组
	osg::ref_ptr<osg::Vec4Array> cr = new osg::Vec4Array;
	geom->setColorArray(cr.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	cr->push_back(osg::Vec4(1.0f, 0.0f, 0.0f,1.0f));
	cr->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cr->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	cr->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//唯一法线数组
	osg::ref_ptr<osg::Vec3Array> norm= new osg::Vec3Array;
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	norm->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	//绘制四边形---
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	//添加几何体返回----
	osg::ref_ptr <osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());

	return geode;
}

osg::ref_ptr<osg::Node> createQuad()
{
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array;
	v->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
	v->push_back(osg::Vec3(1.0f, 0.0f, 1.0f));
	v->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
	geom->setVertexArray(v.get());
	//创建颜色数组
	osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array;
	geom->setColorArray(vc.get());
	geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//添加数据
	vc->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vc->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	vc->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//设置颜色数组setColorArray(Array *array)
	//设置颜色的绑定方式setColorBinding(AttributeBinding ab)为单个顶点
 
	//创建法线数组
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
	geom->setNormalArray(nc.get());
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	//添加法线
	nc->push_back(osg::Vec3(0.0f, -1.0f, 0.0f));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(geom.get());
	return geode;
}

osg::ref_ptr<osg::Node> createOsg3()
{
osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
vertices->push_back( osg::Vec3(0.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(1.0f, 0.0f, 0.0f) );
vertices->push_back( osg::Vec3(1.0f, 0.0f, 1.0f) );
vertices->push_back( osg::Vec3(0.0f, 0.0f, 1.0f) );
osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
normals->push_back( osg::Vec3(0.0f,-1.0f, 0.0f) );
osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
colors->push_back( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );
colors->push_back( osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) );
colors->push_back( osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) );
colors->push_back( osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) );
osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
quad->setVertexArray( vertices.get() );
quad->setNormalArray( normals.get() );
quad->setNormalBinding( osg::Geometry::BIND_OVERALL );
quad->setColorArray( colors.get() );
quad->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
quad->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4) );
osg::ref_ptr<osg::Geode> geod = new osg::Geode;
geod->addDrawable( quad.get() );
return geod;
}
osg::ref_ptr<osg::Node> createTag()
{
	const double z = 0.;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    v->push_back(osg::Vec3(800,100.0,z + 0.1));
    v->push_back(osg::Vec3(1200,100.0,z + 0.1));
    v->push_back(osg::Vec3(1200,140.0,z + 0.1));
    v->push_back(osg::Vec3(800.0,140.0,z + 0.1));
    geom->setVertexArray(v.get());
    osg::ref_ptr<osg::Vec3Array> n=new osg::Vec3Array();
    n->push_back(osg::Vec3(0.0,0.0,-1.0));
    geom->setNormalArray(n.get());
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    osg::ref_ptr<osg::Vec4Array> c=new osg::Vec4Array();
    c->push_back(osg::Vec4(1.0,0.0,0.0,0.2));
    c->push_back(osg::Vec4(1.0,0.0,0.0,0.2));
    c->push_back(osg::Vec4(1.0,0.0,0.0,0.2));
    c->push_back(osg::Vec4(1.0,0.0,0.0,0.2));
    geom->setColorArray(c.get());
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
    osg::ref_ptr<osg::Geode> geode2 = new osg::Geode;
    geode2->addDrawable(geom.get());
	//osg::ref_ptr<osg::MatrixTransform> tranf2 =new osg::MatrixTransform();
	//tranf2->addChild(geode2); 
	return geode2;
}
//读写并View
int main()
{
	//osg::ref_ptr<osg::Node> root = createSceneGraph();//创建场景图形
	//osg::ref_ptr<osg::Node> root = createQuad();//创建场景图形
	osg::ref_ptr<osg::Node> root = createTag();//创建场景图形
	//osg::ref_ptr<osg::Node> root = createOsg3();//创建场景图形
/*	
	if (!root.valid())
		osg::notify(osg::FATAL) << "Fail in createSceneGraph" << std::endl;
    //从保存为osg文件到项目路径下---
	bool result = osgDB::writeNodeFile(*(root.get()), "simple.osg");
	if(!result)
		osg::notify(osg::FATAL) << "Fail in writeNode" << std::endl;
*/
	osgViewer::Viewer viewer;
	//从保存的路径下读取对应的Osg Ascii文件----
	//viewer.setSceneData(osgDB::readNodeFile("F:OSG\\exam01\\simple.osg"));	
	viewer.setSceneData(root);
	return  viewer.run();
}
