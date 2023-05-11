--[[
add_rules("mode.debug", "mode.release")
set_languages("c++20")
add_includedirs(".")
add_includedirs("c:/dev/vs2022/include")
add_includedirs("c:/dev/src/osgearth3rd_3_3")
add_includedirs("c:/dev/src/osgearth3rd_3_3/third_party/imgui")
add_linkdirs("c:/dev/vs2022/lib")
--]]
target("wind_viewer")
    set_kind("binary")
    add_links("osg", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil", "osgShadow")
	add_links("glew32")
	add_syslinks("OpenGL32", "Shell32", "Advapi32")
    add_files("wind_viewer.cpp", "wind_scene.cpp", "wind_shader.cpp")

