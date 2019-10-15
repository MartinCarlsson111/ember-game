#version 330 core
in vec2 TexCoord;
out vec4 frag_color;
uniform sampler2D albedoMap;

void main()
{
	frag_color = texture(albedoMap, TexCoord);// vec4(1.0f, 0.5f, 0.3f, 0.5f);
}
