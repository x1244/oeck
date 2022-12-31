#version 330 
uniform mat4 osg_ModelViewProjectionMatrix; 
uniform mat4 osg_ModelViewMatrix; 
uniform mat3 osg_NormalMatrix; 
uniform vec3 ecLightDir; 
uniform float osg_FrameTime;

in vec4 osg_Vertex; 
in vec4 osg_Color;
in vec3 osg_Normal; 
in vec4 osg_MultiTexCoord0;
out vec4 color; 
out vec2 texCoord;
 
void main() 
{ 
    vec3 ecNormal = normalize( osg_NormalMatrix * osg_Normal ); 
    float diffuse = max( dot( ecLightDir, ecNormal ), 0. ); 
//  color = vec4(diffuse*osg_Color.rgb, 1.0); //dumptruck
    color = vec4( vec3( diffuse ), 1. );      //cow
    

//cow
    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex; 
    
    vec4 pos = normalize(osg_ModelViewMatrix*osg_Vertex);
    pos = pos/pos.w;
    vec3 e = normalize(ecLightDir - pos.xyz);
    texCoord = reflect(-ecLightDir, ecNormal).xz; //环境纹理
    
//    texCoord = osg_MultiTexCoord0.xy; //采样纹理
} 
