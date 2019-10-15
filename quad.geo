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
    highp int index = int(SpriteIn[0].data.z);
    highp int nWidth = int(tileMapData.z);
    int yIndex = index % nWidth;
    int xIndex = index / nWidth;

    gl_Position = vp *(gl_in[0].gl_Position + vec4(-spriteWidth, -spriteHeight, 0, 0));
    TexCoord = vec2((xIndex+1) * tileMapData.x, tileMapData.y * (yIndex+1));
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(spriteWidth, -spriteHeight, 0, 0)); 
    TexCoord = vec2(xIndex * tileMapData.x, tileMapData.y * (yIndex+1));
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(-spriteWidth, spriteHeight, 0, 0));
    TexCoord = vec2((xIndex+1) * tileMapData.x, tileMapData.y * yIndex); 
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(spriteWidth, spriteHeight, 0, 0));
    TexCoord = vec2(xIndex * tileMapData.x, tileMapData.y * yIndex);
    EmitVertex();
}