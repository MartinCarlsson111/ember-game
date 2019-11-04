#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;
uniform mat4 vp;
uniform vec4 tileMapData;
out vec2 TexCoord;


in SpriteInfo{
   vec4 data;
} SpriteIn[];

void main()
{
    float spriteWidth = SpriteIn[0].data.x;
    float spriteHeight = SpriteIn[0].data.y;
    float halfHeight = 0.35;
    float index = SpriteIn[0].data.z + 1;

    gl_Position = vp *(gl_in[0].gl_Position + vec4(0, -halfHeight, 0, 0));
    TexCoord = vec2(index * 0.5, 1.0);
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(spriteWidth , -halfHeight, 0, 0)); 
    TexCoord = vec2(index * 0.5 , 0.0);
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(0, spriteHeight-halfHeight, 0, 0));
    TexCoord = vec2((index-1) * 0.5, 1.0); 
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(spriteWidth, spriteHeight-halfHeight, 0, 0));
    TexCoord = vec2((index-1) * 0.5, 0.0);
    EmitVertex();
}