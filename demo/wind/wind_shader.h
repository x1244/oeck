/**\file 风场着色器
 *
 * \brief 风场着色器GLSL片段
 * \author x1244 <x11244@126.com>
 */
#pragma once
#include <string>
#include <osg/Geode>
#include <osg/Program>
#include <osg/Shader>
#include <osg/StateSet>
#include <osg/Texture>
namespace ick{
/**\brief 应用着色器
 *
 * \param ss - 状态机
 * \param vert - 顶点着色器
 * \param frag - 片元着色器
 */
void applyShader(osg::StateSet* ss, const std::string& vert
    , const std::string& frag);
/**\brief 创建纹理四边形
 *
 * \param w - 四边形宽
 * \param h - 四边形高
 * \param tex - 纹理
 */
osg::ref_ptr<osg::Geode> createQuadWithTexture(float w, float h
    , osg::Texture* tex);
/**\brief 简单纹理顶点着色器
 */
const std::string simple_vert(R"(#version 430 
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 texCoord;
uniform mat4 osg_ModelViewProjectionMatrix;
out vec2 tex;
void main()
{
	tex = texCoord;
	gl_Position = osg_ModelViewProjectionMatrix*vec4(vPos, 1.0);
}
)");
/**\brief 简单纹理片元着色器
 */
const std::string simple_frag(R"(#version 430
in vec2 tex;
uniform sampler2D baseTexture;
out vec4 fragData; 
void main() 
{ 
    fragData = texture(baseTexture, tex);
}
)");
/**\brief 精灵一顶点着色器
 */
const std::string sprite_1_vert(R"(#version 430
uniform float osg_FrameTime;
uniform float osg_DeltaFrameTime;
uniform mat4 osg_ModelViewProjectionMatrix;
uniform float mini; //最小风速
uniform float mpx;  //风速倍数
uniform float pt; 
uniform float widths;
layout (binding = 0) uniform sampler2D uvTexture;
layout (binding = 1) uniform sampler2D spriteTexture;
out vec4 tex;
/**\brief 随机数发器
 */
float random(vec2 uv)
{
    //对比 fract(cos(dot(uv, vec2(12.9898, 78.233)))*43758.5453123)
    //下述取值更合适
    return fract(cos(dot(uv, vec2(1.29898, 7.8233)))*43758.5453123);
}
vec2 random2(vec2 uv)
{
    return vec2(random(uv), random(uv.yx));
}
void main()
{
    float unit = gl_InstanceID/widths;
    float spx = fract(unit);
    float spy = floor(unit)/widths;
	vec4 coord = texture(spriteTexture, vec2(spx, spy));
    //超时
	if(coord.w < 0.01){
	    coord.w = 20.0 + 2.0*random(vec2(gl_InstanceID, osg_DeltaFrameTime));
        float seedx = cos(coord.x);
        if(coord.x == 0.0){
            seedx = spy;
        }
        float seedy = fract((gl_InstanceID + 1024.)*osg_DeltaFrameTime);
	    coord.xy = random2(vec2(seedx, seedy));
	}
	vec2 uv = texture(uvTexture, coord.xy).rg;
	coord.z = length(uv);
	vec2 offset = coord.z*(mini + uv*mpx)*0.00002; //转换到[0, 1]
	coord.xy = fract(coord.xy + offset);
	coord.w = coord.w - osg_DeltaFrameTime;
	tex = coord;
	gl_Position = osg_ModelViewProjectionMatrix*vec4(spx, 0.0, spy, 1.0);
}
)");
/**\brief 精灵一片元着色器
 */
const std::string sprite_1_frag(R"(#version 430
in vec4 tex;
layout (location = 0) out vec4 frag1; 
layout (location = 1) out vec4 frag2; 
void main() 
{ 
    frag1 = tex;
    frag2 = vec4(tex.xy, 0.0, 1.0);
}
)");
/**\brief 精灵二顶点着色器
 */
const std::string sprite_2_vert(R"(#version 430
uniform mat4 osg_ModelViewProjectionMatrix;
uniform float widths;
uniform float pt; 
layout (binding = 0) uniform sampler2D spriteTexture;
out vec4 tex;
void main()
{
    float unit = gl_InstanceID/widths;//单元索引
    float u = fract(unit);
    float v = floor(unit)/widths;
	tex = texture(spriteTexture, vec2(u, v));
	gl_Position = osg_ModelViewProjectionMatrix*vec4(u, 0.0, v, 1.0);
}
)");
/**\brief 精灵二片元着色器
 * 
 * 交换纹理
 */
const std::string sprite_2_frag(R"(#version 430
in vec4 tex;
layout (location = 0) out vec4 frag1; 
layout (location = 1) out vec4 frag2; 
void main() 
{ 
    frag1 = tex;
    frag2 = vec4(tex.xyz, 1.0);
}
)");
/**\brief 风场顶点着色器一
 */
const std::string wind_1_vert(R"(#version 430
uniform mat4 osg_ModelViewProjectionMatrix;
uniform float widths;
uniform float pt; 
layout (binding = 0) uniform sampler2D spriteTexture;
out vec4 tex;
void main()
{
	gl_PointSize = pt;
    float unit = gl_InstanceID/widths;
    float u = fract(unit);
    float v = floor(unit)/widths;
	tex = texture(spriteTexture, vec2(u, v));
	gl_Position = osg_ModelViewProjectionMatrix*vec4(tex.x, 0.0, tex.y, 1.0);
}
)");
/**\brief 风场片元着色器一
 */
const std::string wind_1_frag(R"(#version 430
vec3 colorAt(float v)
{
    //多留了一些，避免采样不到
    //为什么需要到呢？
    float[24] colors = float[24](0.443137255, 0.682352941, 0.274509804, 
                           0.443137255, 0.682352941, 0.274509804, 
                           0.588235294, 0.717647059, 0.266666667, 
                           0.768627451, 0.8, 0.219607843, 
                           0.921568627, 0.882352941, 0.164705882, 
                           0.917647059, 0.690196078, 0.149019608,
                           0.917647059, 0.690196078, 0.149019608,
                           0.917647059, 0.690196078, 0.149019608);
    vec3 color;
    int ix0 = int((floor(v*5.0))*3.0);
    color = vec3(colors[ix0], colors[ix0 + 1], colors[ix0 + 2]);
    return color;
}
in vec4 tex;
layout (location = 0) out vec4 frag1; 
layout (location = 1) out vec4 frag2; 
void main() 
{ 
    frag1 = vec4(colorAt(tex.z), 1.0);
    frag2 = vec4(tex.zzz, 1.0);
}
)");
/**\brief 风场顶点着色器二
 */
const std::string wind_2_vert(R"(#version 430
uniform float widths;
uniform float pt; 
uniform float h;
uniform mat4 osg_ModelViewProjectionMatrix;
layout (binding = 0) uniform sampler2D spriteTexture;
out vec4 tex;
//平面坐标转地心系XYZ
vec3 toECEF(vec2 uv)
{
    const float _a = 6378137.0;
    const float _e2 = 0.006694380047;
    //from u[0, 1]   -> lon [-180., 180.]
    //from v[0, 1] -> lat [-90., 90.]
    float lon = radians(-180.0 + uv.x*360.0);
    float lat = radians(-90.0 + uv.y*180.0);
    float sin_lmd = sin(lon);
    float cos_lmd = cos(lon);
    float sin_b = sin(lat);
    float cos_b = cos(lat);
    float N = _a/sqrt(1. - _e2*pow(sin_b, 2.0));
    float x = (N + h)*cos_b*cos_lmd;
    float y = (N + h)*cos_b*sin_lmd;
    float z = (N*(1.0 - _e2) + h)*sin_b;
    return vec3(x, y, z);
}
void main()
{
	gl_PointSize = pt;
    float unit = gl_InstanceID/widths;
    float u = fract(unit);
    float v = floor(unit)/widths;
	tex = texture(spriteTexture, vec2(u, v));
	vec3 xyz = toECEF(tex.xy);
	gl_Position = osg_ModelViewProjectionMatrix*vec4(xyz, 1.0);
}
)");
}// namespace ick
