set_project("osgearth-ck")
set_version("0.0.1", {build = "%Y%m%d%H%M"})
set_languages("cxx14")
target("oeck")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer")
    add_defines("WIN32")
    add_files("src/osgearth_viewer.cpp")
    add_installfiles("earthfile/simple.earth", {prefixdir = "bin"})
    

