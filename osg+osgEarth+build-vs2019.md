# VS2019

主要使用工具
 - `x64 Native Tools Command Prompt for VS 2019`
 - `xmake` https://github.com/xmake-io/xmake
 - `cmake` https://cmake.org/

## sqlite3

sqlite官网https://sqlite.org/download.html，下载sqlite3, https://sqlite.org/2020/sqlite-amalgamation-3310100.zip，要下载amalgamation版的

创建一个xmake.lua

```
target("sqlite3")
    set_kind("binary")
    add_files("*.c")

target("libsqlite3")
    set_kind("static")
    set_basename("sqlite3")
    add_headerfiles("*.h")
    add_files("sqlite3.c")   
```

放置在`sqlite3`源码目录

```
> cd /path/to/sqlite3
sqlite3> xmake f -c 
sqlite3> xmake build
sqlite3> xmake install -o c:/tools/vs2019
```

## tiff

官网http://www.libtiff.org/下载最新版http://download.osgeo.org/libtiff/tiff-4.1.0.zip

```
> cd /path/to/tiff
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_INSTALL_PREFIX=C:/tools/vs2019 .. 
buildms> msbuild ALL_BUILD.vcxproj /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## curl

curl在github有工程https://github.com/curl/curl.git

commit:7bc709f670237fbee49e24bc96ec3f190b8fc539

```
> cd /path/to/curl
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_INSTALL_PREFIX=C:/tools/vs2019 ..
buildms> msbuild ALL_BUILD.vcxproj /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```



## jpeg

xmake-repo上有libjpeg的构建包。参考其实现，制作jpeg编译包，放入jpeg目录

```
target("jpeg")
    set_kind("static")
    add_files("jaricom.c", "jcapimin.c", "jcapistd.c", "jcarith.c", "jccoefct.c", "jccolor.c")
    add_files("jcdctmgr.c", "jchuff.c", "jcinit.c", "jcmainct.c", "jcmarker.c", "jcmaster.c")
    add_files("jcomapi.c", "jcparam.c", "jcprepct.c", "jcsample.c", "jctrans.c", "jdapimin.c")
    add_files("jdapistd.c", "jdarith.c", "jdatadst.c", "jdatasrc.c", "jdcoefct.c", "jdcolor.c")
    add_files("jddctmgr.c", "jdhuff.c", "jdinput.c", "jdmainct.c", "jdmarker.c", "jdmaster.c")
    add_files("jdmerge.c", "jdpostct.c", "jdsample.c", "jdtrans.c", "jerror.c", "jfdctflt.c")
    add_files("jfdctfst.c", "jfdctint.c", "jidctflt.c", "jidctfst.c", "jidctint.c", "jquant1.c")
    add_files("jquant2.c", "jutils.c", "jmemmgr.c", "jmemansi.c")
    if is_plat("windows") then
        add_configfiles("jconfig.txt", {filename = "jconfig.h"})
    else
        add_configfiles("jconfig.vc", {filename = "jconfig.h"})
    end
    add_includedirs("$(buildir)", {public = true})
    add_headerfiles("jerror.h", "jmorecfg.h", "jpeglib.h", "$(buildir)/jconfig.h")

```

```
> cd /path/to/jpeg
jpeg> xmake
jpeg> xmake install -o C:\tools\vs2019
```

## zlib

zlib官网http://www.zlib.net/，选一个下载http://www.zlib.net/zlib1211.zip

```
> cd /path/to/zlib
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_INSTALL_PREFIX=C:/tools/vs2019 ..
buildms> msbuild ALL_BUILD.vcxproj /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## png

libpng官网http://www.libpng.org/pub/png/libpng.html，选一个下载http://prdownloads.sourceforge.net/libpng/lpng1637.zip?download

```
> cd /path/to/png
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_INSTALL_PREFIX=C:/tools/vs2019 -DPNG_USER_MEM_SUPPORTED=1 -DZLIB_INCLUDE_DIR=C:/tools/vs2019/include -DZLIB_LIBRARY=C:/tools/vs2019/lib/zlib.lib ..
buildms> msbuild ALL_BUILD.vcxproj /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## freetype

官网https://www.freetype.org/download.html，找个链接下载https://download.savannah.gnu.org/releases/freetype/freetype-2.10.0.tar.gz

```
> cd /path/to/freetype
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" -DZLIB_INCLUDE_DIR=C:/tools/vs2019/include -DZLIB_LIBRARY=C:/tools/vs2019/lib/zlib.lib -DPNG_PNG_INCLUDE_DIR="c:/tools/vs2019/include" -DPNG_LIBRARY="c:/tools/vs2019/lib/libpng16.lib" ..
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## proj

~~commit:4f1a05aa9589729b53a305353c30691aa733964f~~

`commit:e8a535ee02e7170bf9f16ff0570c07ee0a3b7ec0`

````
> cd /path/to/PROJ
> mkdir buildms
> cd buildms
buildms> cmake CXXFLAGS="/MD" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/tools/vs2019" -DTIFF_INCLUDE_DIR="C:/tools/vs2019/include" -DBUILD_SHARED_LIBS=YES -DBUILD_TESTING=no -DTIFF_LIBRARY="C:/tools/vs2019/lib/tiff.lib" -DCURL_INCLUDE_DIR="C:/tools/vs2019/include" -DCURL_LIBRARY="C:/tools/vs2019/lib/libcurl_imp.lib" -DSQLITE3_INCLUDE_DIR="C:/tools/vs2019/include" -DSQLITE3_LIBRARY="C:/tools/vs2019/lib/sqlite3.lib" ..
buildms> msbuild ALL_BUILD.vcxproj /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
````

打开项目，对`bin_projsync`和`proj`项目右键属性，[配置属性]=>[链接器]=>[输入]=>[忽略特定默认库]，添加`libcmt.lib`，然后手动生成这两项。

## geos

geos在github有工程，https://github.com/libgeos/geos.git

~~commit:86c21dc435d5f58d70a8dbd1a90cfd8845615e4e~~

`commit 47edc7cb2476980235c36d9060a0953610364a71 `

修改`geos\tools\astyle\ASLocalizer.cpp`文件，将编码转为UTF8。

```
> cd /path/to/geos
> mkdir buildms
> cd buildms
> buildms> cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" -DBUILD_TESTING=NO ..
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
buildms> msbuild capi/INSTALL.vcxproj /p:Configuration="Release"
```

## gdal

参考https://blog.csdn.net/weixin_42141589/article/details/94357199?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-3&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-3

~~commit:19e5e4eb15de3f7bf353e1c5f5cf7df896f937f9~~  

`commit a5f6d1ce6cab00ca4acb5df04ac8ecad5f5c4406 `

编辑gdal/nmake.opt

找到`GDAL_HOME =`设置生成目录`GDAL_HOME = "C:\tools\vs2019"`

`python`配置

`PYDIR   =	"C:\soft\Python38"`

修改`CPLLIB`

```
CPLLIB	=    $(GDAL_ROOT)\port\cpl.lib
```

找到`PROJ_INCLUDE` `PROJ_LIBRARY`

```
PROJ_INCLUDE = -Ic:\tools\vs2019\include
PROJ_LIBRARY = c:\tools\vs2019\lib\proj.lib shell32.lib ole32.lib
```

找到`SQLITE_INC` `SQLITE_LIB`

```
SQLITE_INC=-Ic:\tools\vs2019\include
SQLITE_LIB=c:\tools\vs2019\lib\sqlite3.lib
```

找到`CURL_DIR CURL_INC CURL_LIB`

```
CURL_DIR=C:\tools\vs2019 
CURL_INC = -I$(CURL_DIR)\include
CURL_LIB = $(CURL_DIR)\lib\libcurl_imp.lib wsock32.lib wldap32.lib winmm.lib
```

找到`GEOS_DIR`

```
GEOS_DIR=C:\tools\vs2019
GEOS_CFLAGS = -I$(GEOS_DIR)/include -I$(GEOS_DIR)/include/geos -DHAVE_GEOS
GEOS_LIB     = $(GEOS_DIR)/lib/geos_c.lib
```

修改`EXTERNAL_LIBS`，添加

```
EXTERNAL_LIBS =	$(CPLLIB) wbemuuid.lib 
```

使用`x64 Native Tools Command Prompt for VS 2019`

```
> cd gdal
gdal> nmake -f makefile.vc MSVC_VER=1920 WIN64=1
gdal> nmake /f makefile.vc install
gdal> nmake /f makefile.vc devinstall
```

测试4个程序均成功。

## osg

commit:f2f22df8115aee710aa5429bf4965def3734edc7

```
> cd /path/to/OpenSceneGraph
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_PREFIX_PATH="c:/tools/vs2019" -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" -DBUILD_DOCUMENTATION=yes ..
```

在`Plugins fretype`属性的`release`中[链接器->输入->附加依赖项]中添加库`C:\tools\vs2019\lib\libpng16.lib`

```
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

超级漫长等待

### 文档

需要安装`doxygen`和`graphviz`并加入环境变量。

修改`buildms/doc`下的`openscenegraph.doxyfile`中的`DOT_GRAPH_MAX_NODES = 50`为`DOT_GRAPH_MAX_NODES = 500`，配置`SEARCHENGINE           = YES`。

运行`cmd`

```
buildms> cd doc
doc> doxygen openscenegraph.doxyfile
```

## glew

找到工程glew-cmake

commit:9fb23c3e61cbd2d581e33ff7d8579b572b38ee26

```
> cd /path/to/glew
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" ..
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## expat

expat在Github有项目https://github.com/libexpat/libexpat，下载源码https://github.com/libexpat/libexpat.git

commit:e976867fb57a0cd87e3b0fe05d59e0ed63c6febb

```
> cd /path/to/expat
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" ..
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## osgearth

osgearth有Github工程https://github.com/gwaldron/osgearth，下载其源码https://github.com/gwaldron/osgearth.git

## osgearth

osgearth有Github工程https://github.com/gwaldron/osgearth，下载其源码https://github.com/gwaldron/osgearth.git

`commit 342fcadf4c8892ba84841cb5b4162bdc51519e3c` __V3.1.0可以直接编译，更适用__

然后

```
> cd /path/to/osgearth
> mkdir buildms
> cd buildms
buildms> cmake -DCMAKE_PREFIX_PATH="c:/tools/vs2019" -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" ..
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

### 文档

### 文档

修改`docs/Doxyfile`，因为可能在生成图像中有些冲突，所以不使用dot.exe生成，而用原生doxygen生成类层次。

```
PROJECT_NAME           = "osgEarth"
PROJECT_NUMBER         = 3.1.0
OUTPUT_DIRECTORY       = oedocs
HAVE_DOT               = NO
```

```
osgearth>doxygen docs/Doxyfile
```

