#include <memory>
#include <iostream>
#include <icecream.hpp>
#include <ogr_geometry.h>
#include <ogr_spatialref.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/operation/distance/DistanceOp.h>
#include <geos_c.h>
#include <proj.h>
#include <proj/datum.hpp>
using namespace std;
/**\brief OGRSpatialReference坐标系及Geometry相互关系判断
 *
 * 对OGRLinearRing的应用
 */
void func1();
/**\brief OGRSpatialReference坐标系及Geometry相互关系判断
 *
 * 对OGRLinearRing和OGRPolygon的应用
 */
void func2();
/**\brief 点与线段距离和最近点求解
 *
 * geos的LineString和Point应用
 *
 * 对OGRLinearRing和OGRPolygon的应用
 */
void func3();
/**\brief 点与线段距离求解
 *
 * 对geom的C接口的应用
 */
void func4();
/**\brief OGRGeometry的segmentize应用
 *
 * 对OGRGeometry划分线段，是对投影的直接线段化，没有考虑球面
 */
void func5();
/**\brief UTM投影
 */
void func6();
/**\brief 横轴墨卡托投影
 */
void func7();
/**brief PROJ大地问题反解
 *
 * PROJ只提供了大地连线长的方法，未提供大地问题正解方法
 */
void func8();
/**\brief geos获取参考系
 *
 * 获取geos中参考系参数
 */
void func9();
/**\brief 基于geos的大地问题解
 *
 * geos提供了大地问题的正解和反解方法
 */
void func10();
int main(int argc, char** argv)
{
    func10();
    return 0;
}
void func1()
{
    auto srs = make_shared<OGRSpatialReference>("");
    srs->SetWellKnownGeogCS("WGS84");
    IC(srs.get());
#if 0
    char* tx;
    srs->exportToWkt(&tx, nullptr);
    cout <<"SRS:" <<tx <<endl;
    delete tx;
#endif
    IC(srs->GetReferenceCount());
    auto r = make_shared<OGRLinearRing>();
    r->assignSpatialReference(srs.get());
    IC(srs->GetReferenceCount());
    r->addPoint(120., 38.);
    r->addPoint(122., 38.);
    r->addPoint(122., 36.);
    r->addPoint(120., 36.);
    r->closeRings();
    IC(r->getNumPoints());
    IC(r->getSpatialReference());
    cout <<r->exportToWkt() <<endl;
    auto p = make_shared<OGRLinearRing>();//(121., 37.);
    IC(p->IsValid());
    p->assignSpatialReference(srs.get());
    p->addPoint(120.5, 37.2);
    p->addPoint(121.5, 37.5);
    p->addPoint(120.3, 37.3);
    p->closeRings();
    IC(p->getSpatialReference());
    cout <<"p in area ? " <<p->Within(r.get()) <<endl;
    cout <<"r in area ? " <<r->Within(p.get()) <<endl;
    cout <<"p to area : " <<p->Distance(r.get()) <<endl;
}
void func2()
{
    auto srs = make_unique<OGRSpatialReference>("");
    srs->SetWellKnownGeogCS("WGS84");
    IC(OGRGeometryFactory::haveGEOS());
    OGRGeometry* g = OGRGeometryFactory::createGeometry(wkbPolygon);
    auto r = static_cast<OGRPolygon*>(g);
    r->assignSpatialReference(srs.get());
    OGRGeometry* g2 = OGRGeometryFactory::createGeometry(wkbLinearRing);
    auto r2 = static_cast<OGRLinearRing*>(g2);
    r2->addPoint(120., 38.);
    r2->addPoint(122., 38.);
    r2->addPoint(122., 36.);
    r2->addPoint(120., 36.);
    r2->closeRings();
    r->addRing(r2);
    IC(g->IsValid());
    OGRGeometry* p = OGRGeometryFactory::createGeometry(wkbPoint);
    auto z = static_cast<OGRPoint*>(p);
    z->assignSpatialReference(srs.get());
    z->setX(123.5);
    z->setY(37.5);
    IC(z->IsValid());
    cout <<"p in area ? " <<p->Within(r) <<endl;
    cout <<"p to area : " <<p->Distance(r) <<endl;
    cout <<"polygon:" <<r->exportToWkt() <<endl;
    OGRGeometryFactory::destroyGeometry(g);
    OGRGeometryFactory::destroyGeometry(g2);
    OGRGeometryFactory::destroyGeometry(p);
}
void func3()
{
    using namespace geos::geom;
    auto gy = geos::geom::GeometryFactory::create();
    auto cy = gy->getCoordinateSequenceFactory();
    auto cs = cy->create();
    vector<Coordinate> vcs;
    vcs.push_back(Coordinate(120., 34.));
    vcs.push_back(Coordinate(122., 36.));
    vcs.push_back(Coordinate(122.5, 38.));
    vcs.push_back(Coordinate(119.8, 37.8));
    vcs.push_back(Coordinate(120., 34.));
    cs->setPoints(vcs);
    auto p1 = gy->createLineString(cs.release());
    IC(p1->getNumPoints());
    auto p2 = gy->createPoint(Coordinate(121.0, 35.8));
    using geos::operation::distance::DistanceOp;
    DistanceOp op(p2, p1);
    IC(op.distance());
    auto pp = op.nearestPoints();
    IC(pp->size());
    for(int ix = 0; ix < pp->size(); ++ix){
        cout <<pp->getX(ix) <<" " <<pp->getY(ix) <<endl;
    }
}
void func4()
{
    auto h = GEOS_init_r();
    auto cs = GEOSCoordSeq_create_r(h, 5, 2);
    GEOSCoordSeq_setX_r(h, cs, 0, 120.);
    GEOSCoordSeq_setY_r(h, cs, 0, 34.);
    GEOSCoordSeq_setX_r(h, cs, 1, 122.);
    GEOSCoordSeq_setY_r(h, cs, 1, 36.);
    GEOSCoordSeq_setX_r(h, cs, 2, 122.5);
    GEOSCoordSeq_setY_r(h, cs, 2, 38.);
    GEOSCoordSeq_setX_r(h, cs, 3, 119.8);
    GEOSCoordSeq_setY_r(h, cs, 3, 37.8);
    GEOSCoordSeq_setX_r(h, cs, 4, 120.);
    GEOSCoordSeq_setY_r(h, cs, 4, 34.);
    
    auto p1 = GEOSGeom_createLineString_r(h, cs);

    auto cs2 = GEOSCoordSeq_create_r(h, 1, 2);
    GEOSCoordSeq_setX_r(h, cs2, 0, 121.);
    GEOSCoordSeq_setY_r(h, cs2, 0, 35.8);
    auto p2 = GEOSGeom_createPoint_r(h, cs2);
    double dist;
    int rst = GEOSDistance_r(h, p2, p1, &dist);
    IC(dist);

    auto xs = GEOSNearestPoints_r(h, p2, p1);
    IC(xs);
    double x1, y1, x2, y2;
    GEOSCoordSeq_getXY_r(h, xs, 0, &x1, &y1);
    GEOSCoordSeq_getXY_r(h, xs, 1, &x2, &y2);
    IC(x1, y1);
    IC(x2, y2);
    
    auto cs1 = GEOSGeom_getCoordSeq_r(h, p1);
    IC(cs1 == cs);
    GEOSCoordSeq_setY_r(h, cs, 0, 35.2);
    GEOSDistance_r(h, p2, p1, &dist);
    IC(dist);


    GEOSCoordSeq_destroy_r(h, xs);
    GEOSGeom_destroy_r(h, p1);
    GEOSGeom_destroy_r(h, p2);
    //此处因cs附加到p1，不能再删除
//    GEOSCoordSeq_destroy_r(h, cs);
//    GEOSCoordSeq_destroy_r(h, cs2);
    GEOS_finish_r(h);
}
void func5()
{
    auto srs = make_unique<OGRSpatialReference>("");
    srs->SetWellKnownGeogCS("WGS84");
    IC(OGRGeometryFactory::haveGEOS());
    OGRGeometry* g = OGRGeometryFactory::createGeometry(wkbPolygon);
    auto r = static_cast<OGRPolygon*>(g);
    r->assignSpatialReference(srs.get());
    OGRGeometry* g2 = OGRGeometryFactory::createGeometry(wkbLinearRing);
    auto r2 = static_cast<OGRLinearRing*>(g2);
    r2->addPoint(120., 38.);
    r2->addPoint(122., 38.);
    r2->addPoint(122., 36.);
    r2->addPoint(120., 36.);
    r2->closeRings();
    r->addRing(r2);
    IC(g->IsValid());
    g->segmentize(.5);
    cout <<g->exportToWkt() <<endl;
    auto srs2 = g->getSpatialReference();
    IC(srs2 == srs.get());
    char* tx;
    srs2->exportToWkt(&tx, nullptr);
    cout <<"SRS:" <<tx <<endl;
    delete tx;
    OGRGeometryFactory::destroyGeometry(g);
    OGRGeometryFactory::destroyGeometry(g2);
}
void func6()
{
    //投影
    auto sr1 = make_unique<OGRSpatialReference>();
    sr1->SetProjCS("UTM 17");
    sr1->SetWellKnownGeogCS("WGS84");
    sr1->SetUTM(50);
    unique_ptr<OGRSpatialReference> sr2;
    sr2.reset(sr1->CloneGeogCS());
    auto ctf = OGRCreateCoordinateTransformation(sr1.get(), sr2.get());
    IC(ctf);
    double x = 526434.351935, y = 3522210.609046;
    int rst = ctf->Transform(1, &x, &y);
    IC(rst);
    IC_("#.8f", x, y); //31.83526786 117.27936203
    OGRCoordinateTransformation::DestroyCT(ctf);
}
void func7()
{
    //自定义横轴墨卡托投景
    //k_0=1.即为高期-克吕格投影
    auto sr1 = make_unique<OGRSpatialReference>();
    sr1->SetProjCS("myTM");
    string pj4 = "+proj=tmerc +lat_0=0 +lon_0=117 +k_0=0.9996 +x_0=500000 \
                  +datum=WGS84 +units=m";
    sr1->importFromProj4(pj4.c_str());
    unique_ptr<OGRSpatialReference> sr2;
    sr2.reset(sr1->CloneGeogCS());
    auto ctf = OGRCreateCoordinateTransformation(sr1.get(), sr2.get());
    IC(ctf);
    double x = 526434.351935, y = 3522210.609046;
    int rst = ctf->Transform(1, &x, &y);
    IC(rst);
    IC_("#.8f", x, y); //31.83526786 117.27936203
    OGRCoordinateTransformation::DestroyCT(ctf);
}
void func8()
{
    PJ_CONTEXT* C = proj_context_create();
    //使用此定义可以计算
    auto csc = proj_create(C, "+proj=latlong +datum=WGS84");
    //使用此定义无法计算
    //auto csc = proj_create(C, "WGS84");
    IC(csc);
    auto a = proj_coord(proj_torad(122.), proj_torad(16.), 0., 0.);
    auto b = proj_coord(proj_torad(120.), proj_torad(18.), 0., 0.);
    double d = proj_lp_dist(csc, a, b);
    IC(d);
    auto c = proj_geod(csc, a, b);
    IC(c.geod.s, c.geod.a1);
#if 0
    auto s = proj_coord(307151, proj_torad(-43.6082), 0., 0.);
    auto csc2 =  proj_create(C, "+proj=latlong +datum=WGS84");
    auto c2 = proj_geod(csc2, a, s);
    IC(c2.lmd, c2.phi);
    proj_destroy(csc2);
#endif
    proj_destroy(csc);
    proj_context_destroy(C);
}
void func9()
{
    PJ_CONTEXT* C = proj_context_create();
    //使用此定义不可以获得椭球参数
    //auto csc = proj_create(C, "+proj=latlong +datum=WGS84");
    //使用此定义可获取椭球参数
    auto csc = proj_create(C, "WGS84");
    IC(csc);
    IC(proj_is_crs(csc));
    double sa, sb, sf;
    int ss;
    PJ* elli = proj_get_ellipsoid(C, csc);
    int ppp = proj_ellipsoid_get_parameters(C, elli, &sa, &sb, &ss, &sf);
    IC(sa, sb, ss, sf);
    if(proj_is_crs(csc)){
        IC(proj_as_wkt(C, csc, PJ_WKT1_GDAL, nullptr));
    }
    proj_destroy(csc);
    proj_context_destroy(C);

#if 0
    auto esd = osgeo::proj::datum::Ellipsoid::WGS84;
    IC(esd->semiMajorAxis().value()
        , esd->semiMinorAxis()->value()
        , esd->inverseFlattening()->value());
#endif
}
#include <geodesic.h>
void func10()
{
    PJ_CONTEXT* C = proj_context_create();
    auto csc = proj_create(C, "WGS84");
    double a, f;
    PJ* elli = proj_get_ellipsoid(C, csc);
    proj_ellipsoid_get_parameters(C, elli, &a, nullptr, nullptr, &f);
    IC(a, f);
    proj_destroy(csc);
    proj_context_destroy(C);

    struct geod_geodesic g;
    double lat, lon;
    geod_init(&g, a, 1./f);
    double lon1 = 122., lat1 = 16.;
    double lon2 = 120., lat2 = 18.;
    IC_("#.8f", lon1, lat1, lon2, lat2);
    double s, a1, a2;
    //a2是P2->P1的反向，也就是说在P2看P1且对P1为前向的方位角
    geod_inverse(&g, lat1, lon1, lat2, lon2, &s, &a1, &a2);
    IC_("#.8f", s, a1, a2);
    //307150.57997 -43.60824323 -44.19313072
    geod_direct(&g, lat1, lon1, a1+360., s, &lat, &lon, &a2);
    IC_("#.8f", lon, lat, a2);
    //120.0 18.0 -44.19313072
    //geod 
    //PS > geod +ellps=WGS84 -f "%.6f"
    //  16 122 -43.60824323 307150.57997097
    //  18.000000       120.000000      135.806869
    //  16 122 316.39175677 307150.57997097
    //  18.000000       120.000000      135.806869
}
