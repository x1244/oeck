add_rules("mode.debug", "mode.release")
set_languages("c++20")
add_includedirs("c:/dev/vs2022/include")
add_includedirs("c:/dev/src/osgearth3rd_3_3")
add_includedirs("c:/dev/src/osgearth3rd_3_3/third_party/imgui")
add_linkdirs("c:/dev/vs2022/lib")
target("pick")
    set_kind("binary")
    add_links("osg", "osgDB", "osgEarth", "OpenThreads")
    add_links("osgGA", "osgViewer", "osgUtil", "osgShadow")
	add_links("glew32")
	add_syslinks("OpenGL32", "Shell32", "Advapi32")
	add_files("c:/dev/src/osgearth3rd_3_3/third_party/imgui/imgui.cpp")
	add_files("c:/dev/src/osgearth3rd_3_3/third_party/imgui/imgui_demo.cpp")
	add_files("c:/dev/src/osgearth3rd_3_3/third_party/imgui/imgui_draw.cpp")
	add_files("c:/dev/src/osgearth3rd_3_3/third_party/imgui/imgui_tables.cpp")
	add_files("c:/dev/src/osgearth3rd_3_3/third_party/imgui/imgui_widgets.cpp")
	add_files("c:/dev/src/osgearth3rd_3_3/third_party/imgui/backends/imgui_impl_opengl3.cpp")
	add_includedirs("osgEarth/ImGui")
    add_files("osgEarth/ImGui/ImGui.cpp")

    add_files("osgearth_pick.cpp")
