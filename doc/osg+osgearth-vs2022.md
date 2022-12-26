# VS2022编译osg和osgEarth

## 环境

安装VS2022社区版。

使用`xmake`，`cmake`和`MSBuild`，`MSBuild`是在`x64 Native Tools Command Prompt for VS 2022`下运行的。

`x64 Native Tools Command Prompt for VS 2022`不好用，在`Windows Terimal`中使用vc环境更好，注意，用`&`来执行。

```
> & "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
**********************************************************************
** Visual Studio 2022 Developer Command Prompt v17.4.3
** Copyright (c) 2022 Microsoft Corporation
**********************************************************************
[vcvarsall.bat] Environment initialized for: 'x64'
```

设置环境变量

```
> $Env:CMAKE_INCLUDE_PATH="c:/dev/vs2022/include"
> $Env:CMAKE_LIBRARY_PATH="c:/dev/vs2022/lib"
> $Env:PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.34.31933\bin\Hostx64\x64"
> $Env:PATH+=";C:\tools\CMake\bin;c:\dev\vs2022\bin;C:\tools\xmake;C:\tools\Git\bin"
```

## 技巧

修改`~/.gitconfig`，添加

```
[url "ssh://git@github.com/"]
    insteadOf = https://github.com/
```

这样在克隆、更新库时不怎么受限制。

# MT和MD

`MT`是使用系统多线程静态链接，`MD`是使用系统多线程动态链接。同时设置`CMAKE_C_FLAGS_RELEASE`和`CMAKE_CXX_FLAGS_RELEASE`会更好一些。应该是使用静态链接更可靠一些，但是不能保证每个库都一样，所以不必强求，遇到问题解决问题吧。

## MT配置

```
cmake -DCMAKE_C_FLAGS_RELEASE="/MT /utf-8" -DCMAKE_CXX_FLAGS_RELEASE="/MT /utf-8" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:msvcrt.lib" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/dev/vs2022" -DBUILD_SHARED_LIBS=YES ..
```

## MD配置

```
cmake -DCMAKE_C_FLAGS_RELEASE="/MD /utf-8" -DCMAKE_CXX_FLAGS_RELEASE="/MD /utf-8" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/dev/vs2022" -DBUILD_SHARED_LIBS=YES ..
```

# SQLite

从`SQLite`官网下载[amalgamation](https://sqlite.org/amalgamation.html)版本，version 3.40.0。

创建一个xmake.lua

```
target("sqlite3-app")
    set_kind("binary")
    set_basename("sqlite3")
    add_files("*.c")

target("sqlite3")
    set_kind("static")
    set_basename("sqlite3")
    add_headerfiles("*.h")
    add_files("sqlite3.c")   
```

如果是`set_kind("shared")`时，不会生成`sqlite3.lib`。

放置在`sqlite3`源码目录

```
> cd /path/to/sqlite3
sqlite3> xmake f -c --cflags="-DSQLITE_ENABLE_COLUMN_METADATA"
sqlite3> xmake build --rebuild
sqlite3> xmake install -o c:/dev/vs2022
```

顺手把文档安装到`vs2022/share/doc/sqlite-doc`。

或者可以用SQLite的`Makefile.msc`配置来生成，需要下载快照版本https://sqlite.org/snapshot/sqlite-snapshot-202212231449.tar.gz。解压

```
> cd /path/to/sqlite3
> nmake /F Makefile.msc
```

这将生成一个动态库版本的SQLite。

# GLEW

`GLEW`官网https://glew.sourceforge.net/

## 二进制安装

二进制下载https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download，可将其下载来直接安装64位，`glew32.lib`，x64下居然也命名为`glew32.lib`，bin下的两个可执行文件`glewinfo.exe`、`visualinfo.exe`和一个动态链接库`glew32.dll`也安装了。。可以带`Release`目录，也可以将其提取出来。

## 编译安装

下载源码包https://github.com/nigels-com/glew.git。将build目录下的vc15复制一份到vc17，打开vs2022按提示编译，安装，不推荐。

# imgui

`imgui`在`GitHub`上维护https://github.com/ocornut/imgui，下载源码，拷贝到`c:\dev\src`供参考。

其示例代码下载https://www.dearimgui.org/binaries/imgui-demo-binaries-20220504.zip。将其提取出来，重命名为`imgui_*`，拷贝到`c:\dev\bin`下供参考。

```
-a---            2022/5/4    21:09         148480 imgui_binary_to_compressed_c.exe
-a---            2022/5/4    21:08         854016 imgui_example_glfw_opengl2.exe
-a---            2022/5/4    21:08         860672 imgui_example_glfw_opengl3.exe
-a---            2022/5/4    21:08         875008 imgui_example_glfw_vulkan.exe
-a---            2022/5/4    21:08         818176 imgui_example_win32_directx11.exe
-a---            2022/5/4    21:09         815616 imgui_example_win32_directx9.exe
```

# icecream

`icecream`是一款非常流行的调试助手，`GitHub`链接https://github.com/gruns/icecream，只需一个头文件`icecream.hpp`就可以帮助调试了。

# zlib

由官网https://www.zlib.net/下载`zlib`新版https://www.zlib.net/zlib-1.2.13.tar.gz，

```
> cd /path/to/zlib
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# bzip2

`bzip2`是另一款压缩器，`freetype`会用到它。官网https://sourceware.org/bzip2/，在git上的`bzip2`不是其官方维护，官方库在`git clone git://sourceware.org/git/bzip2.git`。用`x64 Native Tools Command Prompt for VS 2022`的`nmake`来构建。

```
> nmake /f makefile.msc
```

生成两个可执行文件`bzip2.exe`和`bzip2recover.exe`，一个库文件`libbz2.lib`，其头文件为`bzlib.h`，手动安装吧。

# LIBZIP

`libzip`官网https://libzip.org/官网，可下载1.9.2版本https://libzip.org/download/libzip-1.9.2.tar.gz。

```
> cd /path/to/libzip
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## png

依赖`zlib`，到libpng官网http://www.libpng.org/pub/png/，导航至http://www.libpng.org/pub/png/libpng.html，下载新版本1.6.39，http://prdownloads.sourceforge.net/libpng/libpng-1.6.39.tar.gz?download

```
> cd /path/to/png
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# jpeg

`jpeg`官网http://www.ijg.org/更新到了9e版本http://www.ijg.org/files/jpegsr9e.zip。

## xmake

xmake-repo上有libjpeg的构建包。参考其实现，并用`jpeg.vcxproj`中获得源文件列表，制作jpeg编译包，放入jpeg目录。这个安装时方便一些。

```
target("jpeg")
    set_kind("static")
    src={"jaricom.c"
, "jcapimin.c"
, "jcapistd.c"
, "jcarith.c"
, "jccoefct.c"
, "jccolor.c"
, "jcdctmgr.c"
, "jchuff.c"
, "jcinit.c"
, "jcmainct.c"
, "jcmarker.c"
, "jcmaster.c"
, "jcomapi.c"
, "jcparam.c"
, "jcprepct.c"
, "jcsample.c"
, "jctrans.c"
, "jdapimin.c"
, "jdapistd.c"
, "jdarith.c"
, "jdatadst.c"
, "jdatasrc.c"
, "jdcoefct.c"
, "jdcolor.c"
, "jddctmgr.c"
, "jdhuff.c"
, "jdinput.c"
, "jdmainct.c"
, "jdmarker.c"
, "jdmaster.c"
, "jdmerge.c"
, "jdpostct.c"
, "jdsample.c"
, "jdtrans.c"
, "jerror.c"
, "jfdctflt.c"
, "jfdctfst.c"
, "jfdctint.c"
, "jidctflt.c"
, "jidctfst.c"
, "jidctint.c"
, "jmemmgr.c"
, "jmemnobs.c"
, "jquant1.c"
, "jquant2.c"
, "jutils.c"
}
    add_files(src)
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
jpeg> xmake install -o C:\dev\vs2022
```

## nmake

使用`nmake`生成工程。

```
> nmake /f makefile.vs setup-v17
```

配置`jpeg.sln`，目录为`x64`，静态库，编译，手动安装。__不推荐__。

# LERC

LERC是一种开源的图像或光栅格式，支持对任何像素类型（不仅仅是RGB或字节）进行快读[编码](https://baike.baidu.com/item/编码/80092?fromModule=lemma_inlink)和[解码](https://baike.baidu.com/item/解码/10944752?fromModule=lemma_inlink)。用户在编码时设置每个[像素](https://baike.baidu.com/item/像素/95084?fromModule=lemma_inlink)的最大压缩误差，因此原始输入图像的精度得以保持（在用户定义的误差范围内）。由`GitHub`下载`https://github.com/Esri/lerc`。

`commit a85da373502139650669a0f1c7ad1e82c43c3a18`

`Date:   Tue Nov 29 15:39:25 2022 -0800`

```
> cd /path/to/lerc
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# TIFF

`LibTIFF`官网http://www.libtiff.org/只更新到了v3.6.1，实际上应该到http://download.osgeo.org/libtiff/下载，实际上它托管在了`gitlab`上https://gitlab.com/libtiff/libtiff。下载最新版本`tiff-4.5.0rc3.tar.xz`。

```
> cd /path/to/tiff
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 .. 
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

文档安装在了`vs2022/share/doc/tiff`下。

# freetype

官网https://freetype.org/，找到更新2.12.1下载https://download.savannah.gnu.org/releases/freetype/freetype-2.12.1.tar.gz

```
> cd /path/to/freetype
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" ..
buildms> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
buildms> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# FFmpeg

`FFmpeg`官网https://ffmpeg.org/，其源码在`GitHub`维护https://github.com/FFmpeg/FFmpeg，不支持`windows`下直接编译。找到一个替代方法，参考https://github.com/ShiftMediaProject/FFmpeg的实现，直接https://github.com/ShiftMediaProject/FFmpeg/releases/download/5.2.r108954/libffmpeg_5.2.r108954_msvc16_x64.zip下载编译好的，但其只支持到了msvc16。那下载源码然后编译呢https://github.com/ShiftMediaProject/FFmpeg.git。

__这个依赖库有点多，先不处理了吧。__

# Expat

`Expat`是一款XML解析器，`GDAL`对其有依赖，官网https://libexpat.github.io/，2022-10-25发布2.5.0版，它其实叫`libexpat`。在`github`上维护了一个源码包https://github.com/libexpat/libexpat/releases，对其https://github.com/libexpat/libexpat/releases/download/R_2_5_0/expat-2.5.0.tar.gz，可直接下载。

```
> cd /path/to/expat
> mkdir build22
> cd build22
build22> cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# openssl

使用`strawberry-perl-5.32.1.1-64bit.msi`安装`perl`，使用`nasm-2.15.05rc2-installer-x64.exe`安装`nasm`，添加环境变量

```
> $Env:PATH+=";C:\Strawberry\perl\bin;C:\Users\conlay\AppData\Local\bin\NASM"
```

下载`openssl`主版本https://github.com/openssl/openssl.git。

> 最新的稳定版本是3.0系列。1.1.1系列是我们的长期支持(LTS)版本，支持到2023年9月11日。所有旧版本(包括1.1.0、1.0.2、1.0.0和0.9.8)现在都不支持，不应该使用。这些旧版本的用户被鼓励尽快升级到3.0或1.1.1。

`commit 98663afce7a909be1518921a9995540308a52462`

因为环境变量问题，在`Windows terimal`中使用不顺利，因此在`x64 Native Tools Command Prompt for VS 2022`中运行（以管理员身份运行）。

切换到`openssl`目录

```
> perl Configure VC-WIN64A --prefix=c:\dev\vs2022 --with-zlib-include=c:\dev\vs2022\include --with-zlib-lib=c:\dev\vs2022\lib zlib-dynamic

Configuring OpenSSL version 3.2.0-dev for target VC-WIN64A
Using os-specific seed configuration
Created configdata.pm
Running configdata.pm
Created makefile.in
Created makefile
Created include\openssl\configuration.h

**********************************************************************
***                                                                ***
***   OpenSSL has been successfully configured                     ***
***                                                                ***
***   If you encounter a problem while building, please open an    ***
***   issue on GitHub <https://github.com/openssl/openssl/issues>  ***
***   and include the output from the following command:           ***
***                                                                ***
***       perl configdata.pm --dump                                ***
***                                                                ***
***   (If you are new to OpenSSL, you might want to consult the    ***
***   'Troubleshooting' section in the INSTALL.md file first)      ***
***                                                                ***
**********************************************************************
> nmake
> nmake test ;这个需要时间较长，可以不做
> nmake install
```

`openssl`自带了一个`crypto`库`libcrypto.lib`和`libcrypto-3-x64.dll`。

还需要手动拷贝库`libcrypto_static.lib`和`libssl_static.lib`，动态库`libssl-3-x64.dll`。

# libgpg-error

`libgpg-error`是`Libgcrypt`的一个依赖库，其官网https://www.gnupg.org/software/libgpg-error/index.html#:~:text=Libgpg-error%20is%20a%20small%20library%20that%20originally%20defined,more%20features%20have%20been%20added%20to%20the%20library%3A指明其获取方式`git clone git://git.gnupg.org/libgpg-error.git`。

__未能在windows下安装。__

# Libgcrypt

`Libgcrypt`官网https://www.gnupg.org/related_software/libgcrypt/，推荐下载` git clone https://dev.gnupg.org/source/gnupg.git`。

__未能在windows下安装。__

# libssh2

`libssh2`官网https://www.libssh2.org/，下载https://github.com/libssh2/libssh2.git。

版本号：`commit 2e2812dde8c1fc9b48eca592823770ab2e601f7a`

日期:`Date:   Tue Dec 13 22:11:13 2022 +0100`

与`cURL`冲突，__不能安装__

```
> cd /path/to/libssh2
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# CryptoPP

`CryptoPP`官网https://cryptopp.com，是`Crypto++`的另一个命名，`GitHub`维护https://github.com/weidai11/cryptopp。

版本号：`commit 2e2812dde8c1fc9b48eca592823770ab2e601f7a`

日期:`Date:   Tue Dec 13 22:11:13 2022 +0100`

用`vs2022`打开`cryptest.sln`，设置Release模式编译`cryptdll`或静态库`crypt.lib`。

这个头文件有点棘手，__没有安装__。

# cURL

更新到主版本`4ab601d93a07cee665ec2458a51fccd0767c03f1`，日期是`Mon Dec 19 17:17:54 2022 +0100`。

主要依赖是`zlib`、`LibPSL`、~~`LibSSH2`~~，__libssh2会导致cURL编译错误__。

```
> cd /path/to/curl
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
-- Could NOT find LibPSL (missing: LIBPSL_LIBRARY LIBPSL_INCLUDE_DIR)
-- Found LibSSH2: C:/dev/vs2022/lib/libssh2.lib (found version "1.8.2")
-- Enabled SSL backends:
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

那个`LibPSL`先不管它了，好像是用不太上。

# Qhull

`QHull`官网http://www.qhull.org/，用于计算凸包，下载http://www.qhull.org/download，这是一个2022.2的更新。

```
> cd /path/to/qhull
> mkdir build22
> cd build22
build22> cmake -DCMAKE_INSTALL_PREFIX=C:/dev/vs2022 ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# OpenSceneGraph

`OpenSceneGraph`又称为`OSG`，其官网http://www.openscenegraph.com/标示为3.6.5版，但其实在`GitHub`上已经发展到了3.7，https://github.com/openscenegraph/OpenSceneGraph。

`commit 2e4ae2ea94595995c1fc56860051410b0c0be605`

`Date:   Thu Dec 1 18:17:31 2022 +0000`

`osgDB`和`Plugins/freetype`没有添加对库`libpng16.lib`的依赖，需手工添加，修改`src/osgDB/CMakeLists.txt`，在`IF( ZLIB_FOUND )`后添加

```
IF( PNG_FOUND )
    ADD_DEFINITIONS( -DUSE_PNG )
    INCLUDE_DIRECTORIES( ${PNG_INCLUDE_DIR} )
    SET(COMPRESSION_LIBRARIES ${COMPRESSION_LIBRARIES} PNG_LIBRARIES)
ENDIF()
```

在`osgPlugins/freetype/CMakeLists.txt`中,`SET(TARGET_ADDED_LIBRARIES osgText )`之前添加

```
FIND_PACKAGE(bzip2)
IF(BZIP2_FOUND)
    SET(TARGET_LIBRARIES_VARS ${TARGET_LIBRARIES_VARS} BZIP2_LIBRARIES)
ENDIF()
```

## GL3支持

按照`osgearth`的建议，应使用GL3版本的`OpenSceneGraph`，需定义` -DOSG_GL3_AVAILABLE=YES`，这将导致很多原来方案行不通。最典型的就是`osgviewer`出来的牛是黑色的，即使是`osgsimplegl3`出来的牛也只是有光照的白色。主要原因是内置渲染管线和变量已经不能用了，尤其是材料这一块。但`OSG`和`osgEarth`都推荐用`GL3`，那就先用上吧，不过在`osgEarth`那边的支持要好一些，地球的渲染还都是正常的。

`osgsimplegl3.cpp`中说要到http://www.opengl.org/registry/的地方下载一个`gl3.h`头文件，然后再放入`<GL3/gl3.h>`中。实际上`khronos`不再维护`gl3.h`这个头文件了，提供了一个`<GL/glcorearb.h>`供大家使用，这个是在`OpenGL 4.3`规格(API Core Profile)的附录`G.2`中说的。`glcorearb.h`有一个依赖的头文件`<KHR/khrplatform.h>`，这是一个很重要的依赖，几乎所有新时代`OpenGL`和`OpenGL ES`编程都要依赖于它。

* 开启

  `OSG_GL3_AVAILABLE`。

* 禁用

```
  OSG_GL1_AVAILABLE
  OSG_GL2_AVAILABLE
  OSG_GLES1_AVAILABLE
  OSG_GLES2_AVAILABLE
  OSG_GL_DISPLAYLISTS_AVAILABLE
  OSG_GL_FIXED_FUNCTION_AVAILABLE
  OSG_GL_MATRICES_AVAILABLE
  OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
  OSG_GL_VERTEX_FUNCS_AVAILABLE
```

因为与`glcoreabr.h`冲突，参考`glcoreabr.h`实现，对`osg/Texture`在修改`GL_EXT_texture_compression_s3tc`相关定义

```
#ifdef GL_EXT_texture_compression_s3tc
#undef GL_EXT_texture_compression_s3tc 
#endif
```

否则编译时提示错误,需要注释掉Texture头文件第60行,因为该宏被glcorearb.h已经定义过了.

```
 43>D:\OpenSceneGraph\src\osg\dxtctool.h(161,14): error C2065: “GL_COMPRESSED_SRGB_S3TC_DXT1_EX
       T”: 未声明的标识符 [D:\OpenSceneGraph\buildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(162,14): error C2065: “GL_COMPRESSED_SRGB_ALPHA_S3TC_D
       XT1_EXT”: 未声明的标识符 [D:\OpenSceneGraph\buildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(163,14): error C2065: “GL_COMPRESSED_SRGB_ALPHA_S3TC_D
       XT3_EXT”: 未声明的标识符 [D:\OpenSceneGraph\buildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(164,14): error C2065: “GL_COMPRESSED_SRGB_ALPHA_S3TC_D
       XT5_EXT”: 未声明的标识符 [D:\OpenSceneGraph\buildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(161,9): error C2051: case 表达式不是常量 [D:\OpenSceneGraph\b
       uildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(162,9): error C2051: case 表达式不是常量 [D:\OpenSceneGraph\b
       uildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(163,9): error C2051: case 表达式不是常量 [D:\OpenSceneGraph\b
       uildg3\src\osg\osg.vcxproj]
    43>D:\OpenSceneGraph\src\osg\dxtctool.h(164,9): error C2051: case 表达式不是常量 [D:\OpenSceneGraph\b
       uildg3\src\osg\osg.vcxproj]
...
   
```

## GL3

```
> cd /path/to/OpenSceneGraph
> mkdir build22
> cd build22
build22> cmake -DCURL_NO_CURL_CMAKE=YES -DOPENGL_PROFILE="GLCORE" -DOSG_GL3_AVAILABLE=YES -DOSG_GL1_AVAILABLE=FALSE -DOSG_GL2_AVAILABLE=FALSE -DOSG_GLES1_AVAILABLE=FALSE -DOSG_GLES2_AVAILABLE=FALSE -DOSG_GL_DISPLAYLISTS_AVAILABLE=FALSE -DOSG_GL_FIXED_FUNCTION_AVAILABLE=FALSE -DOSG_GL_MATRICES_AVAILABLE=FALSE -DOSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE=FALSE -DOSG_GL_VERTEX_FUNCS_AVAILABLE=FALSE -DCMAKE_CXX_FLAGS_RELEASE="/utf-8" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DBUILD_DOCUMENTATION=YES -DCMAKE_INCLUDE_DIRECTORIES="c:/dev/vs2022/include" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## GL2

```
> cd /path/to/OpenSceneGraph
> mkdir build22
> cd build22
build22> cmake -DCURL_NO_CURL_CMAKE=YES -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DBUILD_DOCUMENTATION=YES -DCMAKE_CXX_FLAGS_RELEASE="/utf-8" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## 文档

需要安装`doxygen`和`graphviz`并加入环境变量。

修改`buildms/doc`下的`openscenegraph.doxyfile`中的`DOT_GRAPH_MAX_NODES = 50`为`DOT_GRAPH_MAX_NODES = 500`，配置`SEARCHENGINE           = YES`。

```
build22> cd doc
doc> doxygen openscenegraph.doxyfile
```

拷贝`OpenSceneGraphReferenceDocs`安装帮助。

## osgQt

__未执行__

gitHub找到`openscenegraph/osgQt`
`commit 8fa9e2aed141488fa0818219f29e7ee9c7e667b0`

```
> cd /path/to/osgQt
> mkdir build
> cd build
build> cmake -DCMAKE_PREFIX_PATH="c:/tools/vs2019;C:/Qt/Qt5.12.8/5.12.8/msvc2017_64" -DCMAKE_INSTALL_PREFIX="c:/tools/vs2019" ..
build> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build> msbuild INSTALL.vcxproj /p:Configuration="Release"
build> copy bin/osgviewerQt.exe c:/tools/vs2019/bin
```

在环境变量中加入`C:/Qt/Qt5.12.8/5.12.8/msvc2017_64`，测试`osgviewerQt cow.osgt`

# RPOJ

`PROJ`官网https://proj.org/已升级到9.0以上，在git上最新提交

`commit 23ffb4ba179a9b3bf1debd2749e92f1862b51536`

`Date:   Fri Dec 23 05:44:35 2022 -0500`

````
> cd /path/to/PROJ
> mkdir build22
> cd build22
build22> cmake -DCMAKE_C_FLAGS_RELEASE="/MD /utf-8" -DCMAKE_CXX_FLAGS_RELEASE="/MD /utf-8" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/dev/vs2022" -DBUILD_SHARED_LIBS=YES -DBUILD_TESTING=no ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
````

# GEOS

`GEOS`官网在https://libgeos.org/，在github工程https://github.com/libgeos/geos.git

`commit dd31eb17c6b5283bfee0c3c513192cd257b015cc  `

`Date:   Thu Dec 22 21:38:50 2022 +0100`

```
> cd /path/to/geos
> mkdir build22
> cd build22
build22> cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" -DBUILD_TESTING=NO ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
build22> msbuild capi/INSTALL.vcxproj /p:Configuration="Release"
```

# GeoTIFF

`GeoTIFF`官网https://trac.osgeo.org/geotiff/，`GEOS`推荐外部链接`GeoTIFF`，如果未找到外部库，那就内部构建一个。`GeoTIFF`依赖`TIFF`，官网上推荐下载http://download.osgeo.org/geotiff/libgeotiff/libgeotiff-1.6.0.tar.gz。

```
> cd /path/to/geotiff
> mkdir build22
> cd build22
build22> cmake -DCMAKE_CXX_FLAGS_RELEASE="/MT /utf-8" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/dev/vs2022" -DBUILD_SHARED_LIBS=YES ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

# GDAL

`GDAL`官网https://gdal.org/，github仓库`ssh://git@github.com/OSGeo/gdal.git`。

`commit 7a4abb688f744919b4f6a5467baa8d64d1ae0c8f`

`Date:   Sat Dec 24 05:06:37 2022 -0500`

其3.6版本已支持`CMake`构建。

```
> cd /path/to/gdal
> mkdir build22
> cd build22
build22> cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DCMAKE_CXX_FLAGS_RELEASE="/MD /utf-8" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" -DBUILD_TESTING=NO ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

提示了一大堆有的没的

```
PS D:\gdal\build22> cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DCMAKE_CXX_FLAGS_RELEASE="/MD /utf-8" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" -DBUILD_TESTING=NO ..
-- Building for: Visual Studio 17 2022
-- Selecting Windows SDK version 10.0.22000.0 to target Windows 10.0.19044.
-- The C compiler identification is MSVC 19.34.31937.0
-- The CXX compiler identification is MSVC 19.34.31937.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.34.31933/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.34.31933/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Performing Test test_AVX
-- Performing Test test_AVX - Success
-- Performing Test test_AVX2
-- Performing Test test_AVX2 - Success
-- Could NOT find Python (missing: Python_NumPy_INCLUDE_DIRS NumPy) (found suitable version "3.8.5", minimum required is "3.6")
-- Looking for vsnprintf
-- Looking for vsnprintf - found
-- Looking for getcwd
-- Looking for getcwd - found
-- Looking for fcntl.h
-- Looking for fcntl.h - found
-- Looking for unistd.h
-- Looking for unistd.h - not found
-- Looking for sys/types.h
-- Looking for sys/types.h - found
-- Looking for locale.h
-- Looking for locale.h - found
-- Looking for xlocale.h
-- Looking for xlocale.h - not found
-- Looking for direct.h
-- Looking for direct.h - found
-- Looking for dlfcn.h
-- Looking for dlfcn.h - not found
-- Looking for stdint.h
-- Looking for stdint.h - found
-- Looking for stddef.h
-- Looking for stddef.h - found
-- Check size of int
-- Check size of int - done
-- Check size of unsigned long
-- Check size of unsigned long - done
-- Check size of void*
-- Check size of void* - done
-- Check size of size_t
-- Check size of size_t - done
-- Looking for C++ include atlbase.h
-- Looking for C++ include atlbase.h - found
-- GDAL_VERSION          = 3.7.0dev
-- GDAL_ABI_FULL_VERSION = 37
-- GDAL_SOVERSION        =
-- Found ODBC: odbc32.lib
-- Could NOT find ODBCCPP (missing: ODBCCPP_LIBRARY ODBCCPP_INCLUDE_DIR)
-- Could NOT find MSSQL_NCLI (missing: MSSQL_NCLI_LIBRARY MSSQL_NCLI_INCLUDE_DIR MSSQL_NCLI_VERSION)
-- Could NOT find MSSQL_ODBC (missing: MSSQL_ODBC_LIBRARY MSSQL_ODBC_INCLUDE_DIR MSSQL_ODBC_VERSION)
-- Could NOT find MySQL (missing: MYSQL_LIBRARY MYSQL_INCLUDE_DIR)
-- Found CURL: C:/dev/vs2022/lib/cmake/CURL/CURLConfig.cmake (found version "7.87.1-DEV")
-- Could NOT find Iconv (missing: Iconv_LIBRARY Iconv_CHARSET_LIBRARY Iconv_INCLUDE_DIR)
-- Could NOT find LibXml2 (missing: LIBXML2_LIBRARY LIBXML2_INCLUDE_DIR)
-- Failed to find XercesC (missing: XercesC_LIBRARY XercesC_INCLUDE_DIR XercesC_VERSION)
-- Could NOT find Deflate (missing: Deflate_LIBRARY Deflate_INCLUDE_DIR)
-- Found OpenSSL: C:/dev/vs2022/lib/libcrypto.lib (found version "3.2.0") found components: SSL Crypto
-- Could NOT find CryptoPP (missing: CRYPTOPP_LIBRARY CRYPTOPP_TEST_KNOWNBUG CRYPTOPP_INCLUDE_DIR)
-- Could NOT find ZSTD (missing: ZSTD_DIR)
-- Could NOT find PkgConfig (missing: PKG_CONFIG_EXECUTABLE)
-- Could NOT find ZSTD (missing: ZSTD_LIBRARY ZSTD_INCLUDE_DIR)
-- Could NOT find SFCGAL (missing: SFCGAL_LIBRARY SFCGAL_INCLUDE_DIR)
-- Reading C:/dev/vs2022/cmake/geotiff-config.cmake
-- GeoTIFF configuration, version 1.6.0
--   include directory: ${GeoTIFF_INCLUDE_DIRS}
--   ${GeoTIFF_LIBRARIES} set to shared library
-- Found ZLIB: C:/dev/vs2022/lib/zlib.lib (found version "1.2.13")
-- Found PNG: C:/dev/vs2022/lib/libpng16.lib (found version "1.6.39")
-- Found JPEG: C:/dev/vs2022/lib/jpeg.lib (found version "90")
-- Performing Test HAVE_JPEGTURBO_DUAL_MODE_8_12
-- Performing Test HAVE_JPEGTURBO_DUAL_MODE_8_12 - Failed
-- Could NOT find GIF (missing: GIF_LIBRARY GIF_INCLUDE_DIR)
-- Could NOT find JSONC (missing: JSONC_DIR)
-- Could NOT find JSONC (missing: JSONC_LIBRARY JSONC_INCLUDE_DIR)
-- Could NOT find OpenCAD (missing: OPENCAD_LIBRARY OPENCAD_INCLUDE_DIR)
-- Found QHULL: C:/dev/vs2022/lib/qhull_r.lib
-- Found LERC: C:/dev/vs2022/lib/Lerc.lib
-- Could NOT find BRUNSLI (missing: BRUNSLI_ENC_LIB BRUNSLI_DEC_LIB BRUNSLI_INCLUDE_DIR)
-- Could NOT find libQB3 (missing: libQB3_DIR)
-- Could NOT find Shapelib (missing: Shapelib_INCLUDE_DIR Shapelib_LIBRARY)
-- Could NOT find PCRE2 (missing: PCRE2-8_LIBRARY PCRE2_INCLUDE_DIR)
-- Could NOT find PCRE (missing: PCRE_LIBRARY PCRE_INCLUDE_DIR)
-- Looking for sqlite3_mutex_alloc
-- Looking for sqlite3_mutex_alloc - found
-- Looking for sqlite3_column_table_name
-- Looking for sqlite3_column_table_name - found
-- Looking for sqlite3_rtree_query_callback
-- Looking for sqlite3_rtree_query_callback - found
-- Looking for sqlite3_load_extension
-- Looking for sqlite3_load_extension - found
-- Looking for sqlite3_progress_handler
-- Looking for sqlite3_progress_handler - found
-- Performing Test SQLite3_HAS_NON_DEPRECATED_AUTO_EXTENSION
-- Performing Test SQLite3_HAS_NON_DEPRECATED_AUTO_EXTENSION - Success
-- Found SQLite3: C:/dev/vs2022/lib/sqlite3.lib (found version "3.40.0")
-- Could NOT find SPATIALITE (missing: SPATIALITE_LIBRARY SPATIALITE_INCLUDE_DIR)
-- Could NOT find RASTERLITE2 (missing: RASTERLITE2_LIBRARY RASTERLITE2_INCLUDE_DIR)
-- Could NOT find LibKML (missing: LIBKML_BASE_LIBRARY LIBKML_INCLUDE_DIR LIBKML_DOM_LIBRARY LIBKML_ENGINE_LIBRARY)
-- Could NOT find HDF5 (missing: HDF5_LIBRARIES HDF5_INCLUDE_DIRS C CXX) (found version "")
-- Could NOT find WebP (missing: WEBP_LIBRARY WEBP_INCLUDE_DIR)
-- Could NOT find FreeXL (missing: FREEXL_LIBRARY FREEXL_INCLUDE_DIR)
-- Could NOT find MRSID (missing: MRSID_LIBRARY MRSID_INCLUDE_DIR)
-- Could NOT find Armadillo (missing: ARMADILLO_INCLUDE_DIR)
-- Could NOT find HDF4 (missing: HDF4_df_LIBRARY HDF4_mfhdf_LIBRARY HDF4_INCLUDE_DIR)
-- Could NOT find ECW (missing: ECW_LIBRARY ECWnet_LIBRARY ECWC_LIBRARY NCSUtil_LIBRARY ECW_INCLUDE_DIR)
-- Could NOT find NetCDF (missing: NetCDF_DIR)
-- Could NOT find NetCDF (missing: NETCDF_LIBRARY NETCDF_INCLUDE_DIR)
-- Could NOT find OGDI (missing: OGDI_LIBRARY OGDI_INCLUDE_DIRS)
-- Looking for CL_VERSION_3_0
-- Looking for CL_VERSION_3_0 - not found
-- Looking for CL_VERSION_2_2
-- Looking for CL_VERSION_2_2 - not found
-- Looking for CL_VERSION_2_1
-- Looking for CL_VERSION_2_1 - not found
-- Looking for CL_VERSION_2_0
-- Looking for CL_VERSION_2_0 - not found
-- Looking for CL_VERSION_1_2
-- Looking for CL_VERSION_1_2 - not found
-- Looking for CL_VERSION_1_1
-- Looking for CL_VERSION_1_1 - not found
-- Looking for CL_VERSION_1_0
-- Looking for CL_VERSION_1_0 - not found
-- Could NOT find OpenCL (missing: OpenCL_LIBRARY OpenCL_INCLUDE_DIR)
-- Could NOT find PostgreSQL (missing: PostgreSQL_LIBRARY PostgreSQL_INCLUDE_DIR)
-- Could NOT find FYBA (missing: FYBA_FYBA_LIBRARY FYBA_FYGM_LIBRARY FYBA_FYUT_LIBRARY FYBA_INCLUDE_DIR)
-- Could NOT find LibLZMA (missing: LIBLZMA_LIBRARY LIBLZMA_INCLUDE_DIR)
-- Could NOT find LZ4 (missing: LZ4_LIBRARY LZ4_INCLUDE_DIR LZ4_VERSION)
-- Could NOT find Blosc (missing: BLOSC_LIBRARY BLOSC_INCLUDE_DIR)
-- Could NOT find basisu (missing: basisu_DIR)
-- Could NOT find IDB (missing: IDB_INCLUDE_DIR IDB_IFCPP_LIBRARY IDB_IFDMI_LIBRARY IDB_IFSQL_LIBRARY IDB_IFCLI_LIBRARY)
-- Could NOT find rdb (missing: rdb_DIR)
-- Could NOT find TileDB (missing: TileDB_DIR)
-- Could NOT find OpenEXR (missing: OpenEXR_LIBRARY OpenEXR_UTIL_LIBRARY OpenEXR_HALF_LIBRARY OpenEXR_IEX_LIBRARY OpenEXR_INCLUDE_DIR Imath_INCLUDE_DIR)
-- Could NOT find MONGOCXX (missing: MONGOCXX_INCLUDE_DIR BSONCXX_INCLUDE_DIR MONGOCXX_LIBRARY BSONCXX_LIBRARY)
-- Could NOT find OpenJPEG (missing: OPENJPEG_LIBRARY OPENJPEG_INCLUDE_DIR)
-- Found JNI: C:/Program Files/Java/jdk-15/include  found components: AWT JVM
-- Could NOT find HDFS (missing: HDFS_LIBRARY HDFS_INCLUDE_DIR)
-- Could NOT find Poppler (missing: Poppler_LIBRARY Poppler_INCLUDE_DIR)
-- Could NOT find Podofo (missing: PODOFO_LIBRARY PODOFO_INCLUDE_DIR)
-- Could NOT find Oracle (missing: Oracle_LIBRARY Oracle_INCLUDE_DIR)
-- Could NOT find FileGDB (missing: FileGDB_LIBRARY FileGDB_INCLUDE_DIR)
-- Could NOT find KDU (missing: KDU_INCLUDE_DIR KDU_LIBRARY KDU_AUX_LIBRARY) (found version "")
-- Could NOT find LURATECH (missing: LURATECH_LIBRARY LURATECH_INCLUDE_DIR)
-- Could NOT find Arrow (missing: Arrow_DIR)
-- Could NOT find SWIG (missing: SWIG_EXECUTABLE SWIG_DIR)
-- Found Java: C:/Program Files/Java/jdk-15/bin/java.exe (found version "15.0.0") found components: Runtime Development
-- Could NOT find CSharp (missing: CSHARP_TYPE CSHARP_VERSION CSHARP_COMPILER)
-- Could NOT find BISON (missing: BISON_EXECUTABLE)
-- Could NOT find BISON (missing: BISON_EXECUTABLE)
-- Copying content of D:/gdal/swig/python/gdal-utils to D:/gdal/build22/swig/python/gdal-utils
-- Copying content of D:/gdal/swig/python/gdal-utils/osgeo_utils to D:/gdal/build22/swig/python/osgeo_utils
-- Found Doxygen: C:/tools/doxygen/bin/doxygen.exe (found version "1.8.18") found components: doxygen missing components: dot
--   Target system:             Windows
--   Installation directory:    C:/dev/vs2022
--   C++ Compiler type:         MSVC
--   C compile command line:     C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.34.31933/bin/Hostx64/x64/cl.exe
--   C++ compile command line:   C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.34.31933/bin/Hostx64/x64/cl.exe
--
--   CMAKE_C_FLAGS:              /DWIN32 /D_WINDOWS
--   CMAKE_CXX_FLAGS:              /DWIN32 /D_WINDOWS /EHsc
--   CMAKE_CXX11_STANDARD_COMPILE_OPTION:
--   CMAKE_CXX11_EXTENSION_COMPILE_OPTION:
--   CMAKE_EXE_LINKER_FLAGS:              /machine:x64
--   CMAKE_MODULE_LINKER_FLAGS:              /machine:x64
--   CMAKE_SHARED_LINKER_FLAGS:              /NODEFAULTLIB:libcmt.lib
--   CMAKE_STATIC_LINKER_FLAGS:              /machine:x64
--   CMAKE_C_FLAGS_DEBUG:              /Zi /Ob0 /Od /RTC1
--   CMAKE_CXX_FLAGS_DEBUG:              /Zi /Ob0 /Od /RTC1
--   CMAKE_EXE_LINKER_FLAGS_DEBUG:              /debug /INCREMENTAL
--   CMAKE_MODULE_LINKER_FLAGS_DEBUG:              /debug /INCREMENTAL
--   CMAKE_SHARED_LINKER_FLAGS_DEBUG:              /debug /INCREMENTAL
--   CMAKE_STATIC_LINKER_FLAGS_DEBUG:
--   CMAKE_C_FLAGS_RELEASE:              /O2 /Ob2 /DNDEBUG
--   CMAKE_CXX_FLAGS_RELEASE:              /MD /utf-8
--   CMAKE_EXE_LINKER_FLAGS_RELEASE:              /INCREMENTAL:NO
--   CMAKE_MODULE_LINKER_FLAGS_RELEASE:              /INCREMENTAL:NO
--   CMAKE_SHARED_LINKER_FLAGS_RELEASE:              /INCREMENTAL:NO
--   CMAKE_STATIC_LINKER_FLAGS_RELEASE:
--   CMAKE_C_FLAGS_MINSIZEREL:              /O1 /Ob1 /DNDEBUG
--   CMAKE_CXX_FLAGS_MINSIZEREL:              /O1 /Ob1 /DNDEBUG
--   CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:              /INCREMENTAL:NO
--   CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL:              /INCREMENTAL:NO
--   CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:              /INCREMENTAL:NO
--   CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL:
--   CMAKE_C_FLAGS_RELWITHDEBINFO:              /Zi /O2 /Ob1 /DNDEBUG
--   CMAKE_CXX_FLAGS_RELWITHDEBINFO:              /Zi /O2 /Ob1 /DNDEBUG
--   CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:              /debug /INCREMENTAL
--   CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO:              /debug /INCREMENTAL
--   CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:              /debug /INCREMENTAL
--   CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO:
--
-- Enabled drivers and features and found dependency packages
-- The following features have been enabled:

 * gdal_JPEG, JPEG image format
 * gdal_RAW, Raw formats:EOSAT FAST Format, FARSITE LCP and Vexcel MFF2 Image
 * gdal_GTIFF, GeoTIFF image format
 * gdal_MEM, Read/write data in Memory
 * gdal_VRT, Virtual GDAL Datasets
 * gdal_HFA, Erdas Imagine .img
 * gdal_SDTS, SDTS translator
 * gdal_NITF, National Imagery Transmission Format
 * gdal_GXF, GXF
 * gdal_AAIGRID, Arc/Info ASCII Grid Format.
 * gdal_CEOS, CEOS translator
 * gdal_SAR_CEOS, ASI CEOS translator
 * gdal_XPM, XPM image format
 * gdal_DTED, Military Elevation Data
 * gdal_JDEM, JDEM driver
 * gdal_ENVISAT, Envisat
 * gdal_ELAS, Earth Resources Laboratory Applications Software
 * gdal_FIT, FIT driver
 * gdal_L1B, NOAA Polar Orbiter Level 1b Data Set (AVHRR)
 * gdal_RS2, RS2 -- RadarSat 2 XML Product
 * gdal_ILWIS, Raster Map
 * gdal_RMF, RMF --- Raster Matrix Format
 * gdal_LEVELLER, Daylon Leveller heightfield
 * gdal_SGI, SGI Image driver
 * gdal_SRTMHGT, SRTM HGT File Read Support
 * gdal_IDRISI, Idrisi Raster Format
 * gdal_GSG, Implements the Golden Software Surfer 7 Binary Grid Format.
 * gdal_ERS, ERMapper .ERS
 * gdal_JAXAPALSAR, JAXA PALSAR Level 1.1 and Level 1.5 processed products support
 * gdal_DIMAP, SPOT Dimap Driver
 * gdal_GFF, Ground-based SAR Applitcations Testbed File Format driver
 * gdal_COSAR, COSAR -- TerraSAR-X Complex SAR Data Product
 * gdal_PDS, USGS Astrogeology ISIS Cube (Version 2)
 * gdal_ADRG, ADRG reader and ASRP/USRP Reader
 * gdal_COASP, DRDC Configurable Airborne SAR Processor (COASP) data reader
 * gdal_TSX, TerraSAR-X XML Product Support
 * gdal_TERRAGEN, Terragen&trade; Terrain File
 * gdal_BLX, Magellan BLX Topo File Format
 * gdal_MSGN, Meteosat Second Generation (MSG) Native Archive Format (.nat)
 * gdal_TIL, EarthWatch .TIL Driver
 * gdal_R, R Object Data Store
 * gdal_NORTHWOOD, NWT_GRD/NWT_GRC -- Northwood/Vertical Mapper File Format
 * gdal_SAGA, SAGA GIS Binary Driver
 * gdal_XYZ, ASCII Gridded XYZ
 * gdal_ESRIC, ESRI compact cache
 * gdal_HF2, HF2/HFZ heightfield raster
 * gdal_KMLSUPEROVERLAY
 * gdal_CTG, CTG driver
 * gdal_ZMAP, ZMAP
 * gdal_NGSGEOID, NOAA NGS Geoid Height Grids
 * gdal_IRIS, IRIS driver
 * gdal_MAP, OziExplorer .MAP
 * gdal_CALS, CALS type 1
 * gdal_SAFE, SAFE -- Sentinel-1 SAFE XML Product
 * gdal_SENTINEL2, Driver for Sentinel-2 Level-1B, Level-1C and Level-2A products.
 * gdal_PRF, PHOTOMOD Raster File
 * gdal_MRF, Meta raster format
 * gdal_WMTS, OGC Web Map Tile Service
 * gdal_GRIB, WMO General Regularly-distributed Information in Binary form
 * gdal_BMP, Microsoft Windows Device Independent Bitmap
 * gdal_TGA, TGA
 * gdal_STACTA, STACTA
 * gdal_OGCAPI, OGCAPI
 * gdal_BSB, Maptech/NOAA BSB Nautical Chart Format
 * gdal_AIGRID, Arc/Info Binary Grid Format
 * gdal_ARG, ARG: Azavea Raster Grid
 * gdal_USGSDEM, USGS ASCII DEM (and CDED)
 * gdal_AIRSAR, AirSAR Polarimetric Format
 * gdal_OZI, OZF2/OZFX3 raster
 * gdal_PCIDSK, PCI Geomatics Database File
 * gdal_SIGDEM, Scaled Integer Gridded DEM .sigdem Driver
 * gdal_RIK, RIK -- Swedish Grid Maps
 * gdal_STACIT, STACIT
 * gdal_PDF, Geospatial PDF
 * gdal_PNG, PNG image format
 * gdal_GIF, Graphics Interchange Format
 * gdal_WCS, OGC Web Coverage Service
 * gdal_HTTP, HTTP driver
 * gdal_ZARR, ZARR
 * gdal_DAAS, Airbus DS Intelligence Data As A Service(DAAS)
 * gdal_EEDA, Earth Engine Data API
 * gdal_PLMOSAIC, PLMosaic (Planet Labs Mosaics API)
 * gdal_WMS, Web Map Services
 * gdal_RASTERLITE, Rasterlite - Rasters in SQLite DB
 * gdal_MBTILES, MBTile
 * gdal_PCRASTER, PCRaster CSF 2.0 raster file driver
 * ogr_MEM, Read/write driver for MEMORY virtual files
 * ogr_GEOJSON, GeoJSON/ESRIJSON/TopoJSON driver
 * ogr_TAB, MapInfo TAB and MIF/MID
 * ogr_SHAPE, ESRI shape-file
 * ogr_KML, KML
 * ogr_VRT, VRT - Virtual Format
 * ogr_AVC, AVC
 * ogr_GML, GML
 * ogr_CSV, CSV
 * ogr_DGN, DGN
 * ogr_GMT, GMT
 * ogr_NTF, NTF
 * ogr_S57, S57
 * ogr_TIGER, U.S. Census TIGER/Line
 * ogr_GEOCONCEPT, GEOCONCEPT
 * ogr_GEORSS, GEORSS
 * ogr_DXF, DXF
 * ogr_PGDUMP, PGDump
 * ogr_GPSBABEL, GPSBABEL
 * ogr_EDIGEO, EDIGEO
 * ogr_SXF, SXF
 * ogr_OPENFILEGDB, OPENFILEGDB
 * ogr_WASP, WAsP .map format
 * ogr_SELAFIN, OSELAFIN
 * ogr_JML, JML
 * ogr_VDV, VDV-451/VDV-452/INTREST Data Format
 * ogr_FLATGEOBUF, FlatGeobuf
 * ogr_MAPML, MapML
 * ogr_SDTS, SDTS
 * ogr_GPX, GPX - GPS Exchange Format
 * ogr_SVG, Scalable Vector Graphics
 * ogr_CSW, CSW
 * ogr_PLSCENES, PLSCENES
 * ogr_WFS, OGC WFS service
 * ogr_NGW, NextGIS Web
 * ogr_ELASTIC, ElasticSearch
 * ogr_IDRISI, IDRISI
 * ogr_PDS, Planetary Data Systems TABLE
 * ogr_SQLITE, SQLite3 / Spatialite RDBMS
 * ogr_GPKG, GeoPackage
 * ogr_OSM, OpenStreetMap XML and PBF
 * ogr_VFK, Czech Cadastral Exchange Data Format
 * ogr_MVT, MVT
 * ogr_AMIGOCLOUD, AMIGOCLOUD
 * ogr_CARTO, CARTO
 * ogr_MSSQLSPATIAL, MSSQLSPATIAL
 * ogr_ODBC, ODBC
 * ogr_PGEO, PGEO
 * ogr_XLSX, Microsoft Office Excel(xlsx)
 * ogr_CAD, OpenCAD
 * ogr_ODS, ODS
 * ogr_LVBAG, LVBAG

-- The following OPTIONAL packages have been found:

 * ODBC
   Enable DB support through ODBC
 * OpenSSL
   Use OpenSSL library
 * ZLIB
   zlib (external)
 * JNI
   SWIG_JAVA: Java binding
 * Java
 * Doxygen

-- The following RECOMMENDED packages have been found:

 * EXPAT
   Read and write XML formats
 * GeoTIFF
   libgeotiff library (external)
 * PNG
   PNG compression library (external)
 * JPEG
   JPEG compression library (external)
 * QHULL
   Enable QHULL (external)
 * LERC
   Enable LERC (external)
 * SQLite3
   Enable SQLite3 support (used by SQLite/Spatialite, GPKG, Rasterlite, MBTiles, etc.)
 * GEOS
   Geometry Engine - Open Source (GDAL core dependency)

-- The following features have been disabled:

 * gdal_HEIF, HEIF
 * gdal_MSG, Meteosat Second Generation
 * gdal_NETCDF, NetCDF network Common Data Form
 * gdal_FITS, FITS Driver
 * gdal_RASDAMAN, Rasdaman GDAL driver
 * gdal_HDF5, Hierarchical Data Format Release 5 (HDF5)
 * gdal_GTA, Generic Tagged Arrays
 * gdal_WEBP, WebP
 * gdal_HDF4, Hierarchical Data Format Release 4 (HDF4)
 * gdal_POSTGISRASTER, PostGIS Raster driver
 * gdal_DDS, DirectDraw Surface
 * gdal_KEA, Kea
 * gdal_JP2OPENJPEG, JPEG2000 driver based on OpenJPEG library
 * gdal_TILEDB, TileDB tiledb.io
 * gdal_EXR, EXR support via OpenEXR library
 * gdal_RDB, RIEGL RDB Map Pixel (.mpx) driver
 * gdal_JPEGXL, JPEG-XL
 * gdal_BASISU_KTX2, Basis Universal and KTX2 texture formats
 * gdal_JP2KAK, JPEG-2000 (based on Kakadu)
 * gdal_JPIPKAK, JPIP Streaming
 * gdal_JP2LURA, JPEG-2000 (based on Luratech)
 * gdal_SDE, ESRI ArcSDE Raster
 * gdal_MRSID, Multi-resolution Seamless Image Database
 * gdal_GEOR, Oracle Spatial GeoRaster
 * gdal_ECW, ERDAS JPEG2000 (.jp2)
 * ogr_GMLAS, GMLAS
 * ogr_DWG, DWG
 * ogr_FILEGDB, FileGDB
 * ogr_LIBKML, LibKML
 * ogr_NAS, NAS/ALKIS
 * ogr_SOSI, SOSI:Systematic Organization of Spatial Information
 * ogr_ILI, ILI
 * ogr_MYSQL, MySQL
 * ogr_PG, PostGIS
 * ogr_XLS, Microsoft Office Excel(xls)
 * ogr_MONGODBV3, MongoDB V3
 * ogr_PARQUET, Parquet
 * ogr_ARROW, Arrow
 * ogr_OCI, Oracle OCI
 * ogr_IDB, IDB
 * ogr_OGDI, OGDI
 * ogr_HANA, SAP HANA

-- The following OPTIONAL packages have not been found:

 * Python (required version >= 3.6)
   SWIG_PYTHON: Python binding
 * ODBCCPP
   odbc-cpp library (external)
 * MSSQL_NCLI
   MSSQL Native Client to enable bulk copy
 * MSSQL_ODBC
   MSSQL ODBC driver to enable bulk copy
 * MySQL
   MySQL
 * Iconv
   Character set recoding (used in GDAL portability library)
 * LibXml2
   Read and write XML formats
 * XercesC
   Read and write XML formats (needed for GMLAS and ILI drivers)
 * Deflate
   Enable libdeflate compression library (complement to ZLib)
 * CryptoPP
   Use crypto++ library for CPL.
 * ZSTD, Zstandard - Fast real-time compression algorithm, <https://github.com/facebook/zstd>
   ZSTD compression library
 * SFCGAL
   gdal core supports ISO 19107:2013 and OGC Simple Features Access 1.2 for 3D operations
 * GIF
   GIF compression library (external)
 * JSONC
   json-c library (external)
 * OpenCAD
   libopencad (external, used by OpenCAD driver)
 * BRUNSLI
   Enable BRUNSLI for JPEG packing in MRF
 * libQB3
   Enable QB3 compression in MRF
 * PCRE2
   Enable PCRE2 support for sqlite3
 * PCRE
   Enable PCRE support for sqlite3
 * SPATIALITE
   Enable spatialite support for sqlite3
 * RASTERLITE2
   Enable RasterLite2 support for sqlite3
 * LibKML
 * HDF5
   Enable HDF5
 * WebP
   WebP compression
 * FreeXL
   Enable XLS driver
 * GTA
   Enable GTA driver
 * MRSID
   MrSID raster SDK
 * Armadillo
   C++ library for linear algebra (used for TPS transformation)
 * CFITSIO
   C FITS I/O library
 * HDF4
   Enable HDF4 driver
 * KEA
   Enable KEA driver
 * ECW
   Enable ECW driver
 * NetCDF
   Enable netCDF driver
 * OGDI
   Enable ogr_OGDI driver
 * OpenCL
   Enable OpenCL (may be used for warping)
 * PostgreSQL
 * FYBA
   enable ogr_SOSI driver
 * LibLZMA
   LZMA compression
 * LZ4
   LZ4 compression
 * Blosc
   Blosc compression
 * JXL
   JPEG-XL compression
 * JXL_THREADS
   JPEG-XL threading
 * Crnlib
   enable gdal_DDS driver
 * basisu
   Enable BASISU driver
 * IDB
   enable ogr_IDB driver
 * RASDAMAN
   enable rasdaman driver
 * rdb
   enable RIEGL RDB library
 * TileDB
   enable TileDB driver
 * OpenEXR
   OpenEXR >=2.2
 * MONGOCXX
   Enable MongoDBV3 driver
 * HEIF
   HEIF >= 1.1
 * OpenJPEG
 * HDFS
   Enable Hadoop File System through native library
 * Poppler, A PDF rendering library, <http://poppler.freedesktop.org>
   Enable PDF driver with Poppler (read side)
 * PDFIUM
   Enable PDF driver with Pdfium (read side)
 * Oracle
   Enable Oracle OCI driver
 * TEIGHA
   Enable DWG and DGNv8 drivers
 * FileGDB
   Enable FileGDB (based on closed-source SDK) driver
 * KDU
   Enable KAKADU
 * LURATECH
   Enable JP2Lura driver
 * Arrow
   Apache Arrow C++ library
 * Dotnet
 * CSharp
   SWIG_CSharp: CSharp binding
 * BISON

-- The following RECOMMENDED packages have not been found:

 * SWIG, software development tool that connects programs written in C and C++ with a variety of high-level programming languages., <http://swig.org/>
   Enable language bindings

-- Internal libraries enabled:

 * GIF internal library enabled
 * JSONC internal library enabled
 * OPENCAD internal library enabled


-- Configuring done
-- Generating done
-- Build files have been written to: D:/gdal/build22
```

# osgearth

`osgearth`官网http://docs.osgearth.org/en/latest/， 有Github源码https://github.com/gwaldron/osgearth.git。

`commit a4a03147f90fb3be43e10a0b76903179a1e9ae8a`

`Date:   Wed Dec 14 09:52:24 2022 -0500`

`osgEarth`在内部维护了一个`liblerc`，与官方版本`a85da373502139650669a0f1c7ad1e82c43c3a18`有冲突。提示3个错误：

```
“D:\osgearth\build22\ALL_BUILD.vcxproj”(默认目标) (1) ->
       “D:\osgearth\build22\src\osgEarthDrivers\lerc\osgdb_lerc.vcxproj”(默认目标) (8) ->
       (ClCompile 目标) ->
         D:\osgearth\src\osgEarthDrivers\lerc\ReaderWriterLERC.cpp(178,14): error C2660: “lerc_dec
       ode”: 函数不接受 9 个参数 [D:\osgearth\build22\src\osgEarthDrivers\lerc\osgdb_lerc.vcxproj]
         D:\osgearth\src\osgEarthDrivers\lerc\ReaderWriterLERC.cpp(350,14): error C2660: “lerc_com
       puteCompressedSize”: 函数不接受 9 个参数 [D:\osgearth\build22\src\osgEarthDrivers\lerc\osgdb_lerc.v
       cxproj]
         D:\osgearth\src\osgEarthDrivers\lerc\ReaderWriterLERC.cpp(364,14): error C2660: “lerc_enc
       ode”: 函数不接受 11 个参数 [D:\osgearth\build22\src\osgEarthDrivers\lerc\osgdb_lerc.vcxproj]
```

解决办法就是暂时变更`c:\dev\vs2022`中头文件`LERC`的命名。是否能更有效的修改`ReaderWriterLERC.cpp`源码，适配新的`LERC`呢？这个可行性不大。安装后记得将`LERC`相关的配置改回去。

## GL3

`GL3`按手册完成了`OpenGL3`的升级，也没有了GL3提示。

```
> cd /path/to/osgearth
> mkdir build22
> cd build22
build22> cmake -DCURL_NO_CURL_CMAKE=ON -DOPENGL_PROFILE="GLCORE" -DOSGEARTH_BUILD_ZIP_PLUGIN=YES -DCMAKE_BUILD_TYPE=Release -DOSGEARTH_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=YES -DCMAKE_CXX_FLAGS_RELEASE="/MD" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

## GL2

```
> cd /path/to/osgearth
> mkdir build22
> cd build22
build22> cmake -DCURL_NO_CURL_CMAKE=ON -DOSGEARTH_BUILD_ZIP_PLUGIN=YES -DCMAKE_BUILD_TYPE=Release -DOSGEARTH_BUILD_TESTS=OFF -DBUILD_SHARED_LIBS=YES -DCMAKE_CXX_FLAGS_RELEASE="/MD" -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" ..
build22> msbuild ALL_BUILD.vcxproj /m /p:Configuration="Release"
build22> msbuild INSTALL.vcxproj /p:Configuration="Release"
```

### 版本信息
生成的版本信息

```
> osgearth_version --caps
[osgEarth]  [Registry] Note: GDAL_DATA environment variable is not set
[osgEarth]  [Capabilities] Capabilities:
[osgEarth]  [Capabilities]   osgEarth Version:  3.3.0 build 147
[osgEarth]  [Capabilities]   OSG Version:       3.7.0
[osgEarth]  [Capabilities]   GDAL Version:      3.7.0dev-7a4abb688f
[osgEarth]  [Capabilities]   GEOS Version:      3.12.0dev
[osgEarth]  [Capabilities]   GPU Vendor:        NVIDIA Corporation
[osgEarth]  [Capabilities]   GPU Renderer:      NVIDIA GeForce RTX 2070 SUPER/PCIe/SSE2
[osgEarth]  [Capabilities]   GL/Driver Version: 3.3.0 NVIDIA 526.98 (330)
[osgEarth]  [Capabilities]   GL Core Profile:   yes
```


### cmake提示
`cmake`时给出的一些信息

```
PS D:\osgearth\build22> cmake -DCURL_NO_CURL_CMAKE=ON -DOSGEARTH_BUILD_ZIP_PLUGIN=YES -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=YES -DCMAKE_CXX_FLAGS_RELEASE="/MD" -DOSGEARTH_BUILD_TESTS=OFF -DCMAKE_SHARED_LINKER_FLAGS="/NODEFAULTLIB:libcmt.lib" -DCMAKE_INSTALL_PREFIX="c:/dev/vs2022" ..
-- Building for: Visual Studio 17 2022
-- Selecting Windows SDK version 10.0.22000.0 to target Windows 10.0.19044.
-- The C compiler identification is MSVC 19.34.31937.0
-- The CXX compiler identification is MSVC 19.34.31937.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.34.31933/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.34.31933/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found OpenGL: opengl32
-- Submodule update
-- Found CURL: C:/dev/vs2022/lib/libcurl_imp.lib (found version "7.87.1-DEV")
-- Found GDAL: C:/dev/vs2022/lib/gdal.lib (found version "3.7.0dev")
-- Found GLEW: C:/dev/vs2022/include (found version "2.1.0")
-- Could NOT find Protobuf (missing: Protobuf_LIBRARIES Protobuf_INCLUDE_DIR)
-- Could NOT find WEBP (missing: WEBP_INCLUDE_DIR)
CMake Warning (dev) at C:/tools/CMake/share/cmake-3.25/Modules/FindPackageHandleStandardArgs.cmake:438 (message):
  The package name passed to `find_package_handle_standard_args` (LibZip)
  does not match the name of the calling package (LIBZIP).  This can lead to
  problems in calling code that expects `find_package` result variables
  (e.g., `_FOUND`) to follow a certain pattern.
Call Stack (most recent call first):
  CMakeModules/FindLibZip.cmake:39 (FIND_PACKAGE_HANDLE_STANDARD_ARGS)
  CMakeLists.txt:169 (find_package)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Found LibZip: C:/dev/vs2022/lib/zip.lib
-- Found libzip: C:/dev/vs2022/lib/zip.lib
-- Found GDAL 3.7.0dev
-- Found GLEW C:/dev/vs2022/lib/glew32.lib
-- Found OSG version 3.7.0
-- Creating D:/osgearth/build22/build_include/osgEarth/BuildConfig.h
CMake Warning (dev) at C:/tools/CMake/share/cmake-3.25/Modules/FindPackageHandleStandardArgs.cmake:438 (message):
  The package name passed to `find_package_handle_standard_args` (Git) does
  not match the name of the calling package (git).  This can lead to problems
  in calling code that expects `find_package` result variables (e.g.,
  `_FOUND`) to follow a certain pattern.
Call Stack (most recent call first):
  C:/tools/CMake/share/cmake-3.25/Modules/FindGit.cmake:128 (find_package_handle_standard_args)
  src/osgEarth/CMakeLists.txt:512 (find_package)
This warning is for project developers.  Use -Wno-dev to suppress it.

-- Found Git: C:/tools/Git/bin/git.exe (found version "2.31.1.windows.1")
-- Building osgEarth as a SHARED library
-- INCDIR: C:/dev/vs2022/include
-- Configuring done
-- Generating done
-- Build files have been written to: D:/osgearth/build22
```

## 文档

修改`docs/Doxyfile`，因为生成图像中有冲突，所以不使用`dot.exe`，而用原生`doxygen`生成类层次。

```
osgearth> doxygen docs/Doxyfile
```

生成帮助文件在`docs/html`目录中，拷贝安装。

