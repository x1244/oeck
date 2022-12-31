--> xmake f -c --cxflags="/utf-8"
set_project("osgearth-ck")                          
set_version("0.1.0", {build = "%Y%m%d%H%M"})
add_includedirs("c:/dev/vs2022/include")
add_linkdirs("c:/dev/vs2022/lib")
set_languages("cxx17")
target("viewer")
    set_kind("binary")
    add_links("osg", "osgText", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil")
    add_defines("WIN32")
    add_files("src/earth_viewer.cpp")

includes("demo")