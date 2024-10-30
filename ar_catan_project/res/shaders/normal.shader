#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

out vec3 v_color;

uniform mat4 u_MVP;
uniform mat4 u_normalMat;

void main()
{
	gl_Position = u_MVP * position;
	v_color = normalize(vec3(u_normalMat * normal));
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_color;

void main()
{
	
	color = vec4(v_color,1.0);
};