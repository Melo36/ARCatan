#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

uniform mat4 u_MVP;
uniform mat4 u_normalMat;
uniform mat4 u_model;

out vec3 norm;
out vec3 fragment_position;

void main()
{
	gl_Position = u_MVP * position;

	fragment_position = vec3(u_model * position);
	norm = normalize(vec3(u_normalMat * normal));
};

#shader fragment
#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

struct Light {
	vec4 vector; //(either position or direction depending on w coordinate 1.0 or 0.0
	vec3 color;
	vec3 ambient;
};

#define NR_LIGHTS 5
uniform Light lights[NR_LIGHTS];

// in's from vertex shader
in vec3 fragment_position;  
in vec3 norm;
  
//camera
uniform vec3 view_pos;

//uniform sampler2D texture0;

//out
layout(location = 0) out vec4 color;

vec3 calcLight(Light light, vec3 normal,vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 view_dir = normalize(view_pos - fragment_position);

	vec3 result;
	
	for(int i = 0; i < NR_LIGHTS; i++)
	{
		result += calcLight(lights[i],norm,fragment_position,view_dir);
	}
	
    color = vec4(result,1.0f);
};

vec3 calcLight(Light light, vec3 normal,vec3 fragPos, vec3 viewDir)
{
	//check kind of light
	vec3 light_dir;
	float attenuation = 1.0;
	if(light.vector.w < 0.001)
	{
		//directional light
		light_dir = normalize(vec3(-light.vector));
	}else{
		//point light
		light_dir = normalize(vec3(light.vector)-fragPos);

		float dist = distance(fragPos, vec3(light.vector));
		attenuation = (1.0/(1.0 + dist * 0.007 + pow(dist, 2) * 0.0002));
	}

	//diffuse
	float diff = max(dot(normal, light_dir), 0.0f);
	vec3 diffuse = attenuation * light.color * (diff * material.diffuse);

	//specular
	vec3 halfway_dir = normalize(light_dir + viewDir);

    float specular_value;
	//avoid undefined result of pow when shininess is 0 or smaller
	if(material.shininess <= 0){
		specular_value = 0.1f;
	}else{
		specular_value = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);
	}
    vec3 specular = attenuation * light.color * (specular_value * material.specular); 

	//ambient
	vec3 ambient = attenuation * (light.ambient * material.ambient);
	
	return (ambient + diffuse + specular);
};