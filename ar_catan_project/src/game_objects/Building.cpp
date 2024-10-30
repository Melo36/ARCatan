#include "Building.h"


GOBuilding::GOBuilding(glm::vec4 color, std::vector<std::shared_ptr<Mesh>> meshes, std::shared_ptr<Shader> s, std::shared_ptr<Renderer> renderer, std::vector<Material> overrideMaterials)
	:GameObject(meshes, s, renderer, overrideMaterials), m_color(color), m_buildingType(DefaultBuilding)
{
	this->setScale(glm::vec3(0.0008));
}

GOBuilding::GOBuilding(glm::vec4 color, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> s, std::shared_ptr<Renderer> renderer, Material* overrideMaterial)
	:GameObject(mesh, s, renderer, overrideMaterial), m_color(color), m_buildingType(DefaultBuilding)
{
}

void GOBuilding::setColor(glm::vec4 color)
{
	m_color = color;
}

void GOBuilding::setBuildingType(BuildingType type)
{
	m_buildingType = type;
}

BuildingType GOBuilding::getBuildingType()
{
	return m_buildingType;
}

void GOBuilding::setMesh(std::vector<std::shared_ptr<Mesh>> mesh)
{
	m_meshes = mesh;
}

std::vector<std::shared_ptr<Mesh>> GOBuilding::getMesh()
{
	return m_meshes;
}

void GOBuilding::onRender()
{
	m_shader->bind();
	m_shader->setUniform4f("u_color", m_color.x, m_color.y, m_color.z, m_color.w);

	GameObject::onRender();
	m_shader->setUniform4f("u_color", 0, 0, 0, 0);
}
