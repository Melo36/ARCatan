#pragma once
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <memory>

enum MaterialType{Colored,Mapped,Basic,ShininessOnly};

class Material {
public:
	Material(float shininess);
	Material(float specularStrength, float shininess);
	Material(glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor, float shininess);
	Material(std::shared_ptr<Texture> diffuseMap, std::shared_ptr<Texture> specularMap, float shininess);
	Material(const Material& other);
	//Material(std::vector<std::shared_ptr<Texture>> textures, float shininess, std::shared_ptr<Shader> shader);

	void setMaterialUniforms(std::shared_ptr <Shader> shader);

	glm::vec4 getAmbientColor();

	glm::vec4 getDiffuseColor();

	glm::vec4 getSpecularColor();

	float getShininess();

	float getSpecularStrength();

	Texture* getDiffuseMap();

	Texture* getSpecularMap();
private:

	void setupTextures(std::shared_ptr<Shader> shader);

	MaterialType type;

	glm::vec4 m_ambientColor;

	std::shared_ptr<Texture> m_diffuseMap;
	glm::vec4 m_diffuseColor;

	std::shared_ptr<Texture> m_specularMap;
	glm::vec4 m_specularColor;

	float m_shininess;
	float m_specularStrength;
	bool m_hasMaps;
};