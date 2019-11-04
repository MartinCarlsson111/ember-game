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
    float yIndex = index / nWidth;
    float xIndex = index % nWidth;

    float xWidth = 0.5;
    float yWidth = 1;

    gl_Position = vp *(gl_in[0].gl_Position + vec4((xWidth * -spriteWidth), yWidth *-spriteHeight, 0, 0));
    TexCoord = vec2((xIndex+1.0) * tileMapData.x, tileMapData.y * (yIndex+1.0));
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(xWidth *spriteWidth, yWidth *-spriteHeight, 0, 0)); 
    TexCoord = vec2(xIndex * tileMapData.x, tileMapData.y * (yIndex+1.0));
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(xWidth *-spriteWidth, yWidth * spriteHeight, 0, 0));
    TexCoord = vec2((xIndex+1.0) * tileMapData.x, tileMapData.y * yIndex); 
    EmitVertex();

    gl_Position = vp *(gl_in[0].gl_Position + vec4(xWidth *spriteWidth, yWidth *spriteHeight, 0, 0));
    TexCoord = vec2(xIndex * tileMapData.x, tileMapData.y * yIndex);
    EmitVertex();
}