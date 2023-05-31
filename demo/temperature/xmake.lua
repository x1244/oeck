target("tempe_viewer")
    set_kind("binary")
    add_links("osg", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil", "osgShadow")
	add_links("glew32")
	add_syslinks("OpenGL32", "Shell32", "Advapi32")
    add_files("main.cpp", "tempeLayer.cpp")

