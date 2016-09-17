#version 410 core 

uniform mat4 MVP;
in vec3 vPos;
in vec2 vertexUV;
out vec2 UV;
void main()
{
    gl_Position = MVP * vec4(vPos, 1.0);
    UV = vertexUV;
}
