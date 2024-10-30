#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoords;

out vec2 v_texCoords;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * position;
	v_texCoords = vec2(texCoords.x, 1.0-texCoords.y);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 v_texCoords;

uniform sampler2D cam_texture;

void main()
{
	color = texture(cam_texture, v_texCoords);
};