#include "Material.h"

Material::Material(float shininess)
	:m_hasMaps(false),m_shininess(shininess),type(ShininessOnly)
{
}

Material::Material(float specularStrength, float shininess)
	:m_hasMaps(false), m_specularStrength(specularStrength), m_shininess(shininess),type(Basic)
{
}

Material::Material(glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess)
	:m_hasMaps(false),m_ambientColor(ambientColor), m_diffuseColor(diffuseColor),m_specularColor(specularColor),m_shininess(shininess),m_specularStrength(0.0), type(Colored)
{
}

Material::Material(std::shared_ptr<Texture> diffuseMap, std::shared_ptr<Texture> specularMap, float shininess)
	:m_hasMaps(true), m_diffuseMap(diffuseMap), m_specularMap(specularMap), m_shininess(shininess), m_ambientColor(glm::vec4()), m_diffuseColor(glm::vec4()), m_specularColor(glm::vec4()),m_specularStrength(0.0f), type(Mapped)
{
	
}

Material::Material(const Material& other)
	:m_hasMaps(other.m_hasMaps),type(other.type),m_ambientColor(other.m_ambientColor),m_specularColor(other.m_specularColor), m_diffuseColor(other.m_diffuseColor),m_diffuseMap(other.m_diffuseMap),
	m_specularMap(other.m_specularMap),m_shininess(other.m_shininess),m_specularStrength(other.m_specularStrength)
{
}

void Material::setMaterialUniforms(std::shared_ptr <Shader> shader)
{
	switch (type) {
	case Basic:
		shader->setUniform3f("material.specularStrength", glm::vec3(m_specularStrength));
		break;
	case Colored:
		shader->setUniform3f("material.ambient", glm::vec3(m_ambientColor));
		shader->setUniform3f("material.diffuse", glm::vec3(m_diffuseColor));
		shader->setUniform3f("material.specular", glm::vec3(m_specularColor));
		break;
	case Mapped:
		//set textures from material
		setupTextures(shader);
		break;
	}
	//set shininess for all
	shader->setUniform1f("material.shininess", m_shininess);

}

glm::vec4 Material::getAmbientColor()
{
	if (!m_hasMaps) {
		return m_ambientColor;
	}
	return glm::vec4();
}

glm::vec4 Material::getDiffuseColor()
{
	if (!m_hasMaps) {
		return m_diffuseColor;
	}
	return glm::vec4();
}

glm::vec4 Material::getSpecularColor()
{
	if (!m_hasMaps) {
		return m_specularColor;
	}
	return glm::vec4();
}

float Material::getShininess()
{
	return m_shininess;
}

float Material::getSpecularStrength()
{
	return m_specularStrength;
}

Texture* Material::getDiffuseMap()
{
	if (m_hasMaps) {
		return m_diffuseMap.get();
	}
	return nullptr;
}

Texture* Material::getSpecularMap()
{
	if (m_hasMaps) {
		return m_specularMap.get();
	}
	return nullptr;
}

void Material::setupTextures(std::shared_ptr<Shader> shader)
{
	shader->bind();
	m_diffuseMap->bind();
	shader->setUniform1i("material.texture_d0", 0);
	m_specularMap->bind(1);
	shader->setUniform1i("material.texture_s0", 1);
}
