target("oe-ck")
    set_kind("binary")
    add_includedirs("c:/tools/vs2019/include")
    add_linkdirs("c:/tools/vs2019/lib")
    add_links("osg", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer")
    add_defines("WIN32")
    add_files("src/*.cpp")

-- version
set_version("0.0.1", {build = "%Y%m%d%H%M"})
set_languages("cxx14")

