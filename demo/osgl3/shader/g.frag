#version 330 
uniform sampler2D baseTexturex;
in vec2 texCoord;
in vec4 color; 
out vec4 fragData; 
void main() 
{ 
//    fragData = color; //dumptruck
    fragData = color*texture(baseTexturex, texCoord);//cow
}
