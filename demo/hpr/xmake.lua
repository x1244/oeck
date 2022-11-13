add_rules("mode.debug", "mode.release")
set_languages("c++17")
-- 更新设备俯仰方位跟踪
target("hpr")
    set_kind("binary")
    add_defines("WIN32")
	add_includedirs("c:/tools/vs2019/include")
	add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_links("osgSim")
    add_files("hpr.cpp", "ttp.cpp", "ttp_update.cpp")

