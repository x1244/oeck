set_project("osgearth-ck")
set_version("0.0.1", {build = "%Y%m%d%H%M"})
set_languages("cxx14")
target("oeck")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_defines("WIN32")
    add_files("src/osgearth_viewer.cpp", "src/panorama.cpp", "src/panorama_camera.cpp", "src/earth_loader.cpp")
	add_files("src/pick.cpp", "src/angle_site.cpp")
    add_installfiles("earthfile/simple.earth", {prefixdir = "bin"})
	
target("hud")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_defines("WIN32")
    add_files("src/hud.cpp")
    
target("model")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_defines("WIN32")
    add_files("src/model.cpp")
    add_installfiles("earthfile/simple.earth", {prefixdir = "bin"})

target("quad")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_defines("WIN32")
    add_files("src/rect.cpp")

	
target("elev")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_defines("WIN32")
    add_files("src/elevation.cpp", "src/MouseHandler.cpp")
    add_installfiles("earthfile/simple.earth", {prefixdir = "bin"})
	
target("ctrl")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer")
    add_defines("WIN32")
	add_includedirs("src")
    add_files("src/ctrl.cpp")
    add_installfiles("earthfile/simple.earth", {prefixdir = "bin"})

target("tex")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer")
    add_defines("WIN32")
    add_files("src/tex.cpp")
	
	
target("cone")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer")
    add_defines("WIN32")
    add_files("src/cone.cpp")

target("drag")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgText", "osgDB", "osgManipulator", "OpenThreads")
    add_links("osgGA", "osgViewer")
    add_defines("WIN32")
    add_files("src/drag.cpp")
