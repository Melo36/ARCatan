#pragma once

#include "GameObject.h"
#include "../game_logic/GameBuilding.h"

class GOBuilding : public GameObject {
public:
	GOBuilding(glm::vec4 color, std::vector<std::shared_ptr<Mesh>> meshes, std::shared_ptr <Shader>s, std::shared_ptr<Renderer> renderer, std::vector<Material> overrideMaterials = std::vector<Material>());
	GOBuilding(glm::vec4 color, std::shared_ptr<Mesh> mesh, std::shared_ptr <Shader>s, std::shared_ptr<Renderer> renderer, Material* overrideMaterial = nullptr);

	void setColor(glm::vec4 color);

	void setBuildingType(BuildingType type);

	BuildingType getBuildingType();

	void setMesh(std::vector<std::shared_ptr<Mesh>> mesh);
	std::vector<std::shared_ptr<Mesh>> getMesh();

	void onRender() override;
private:
	glm::vec4 m_color;
	BuildingType m_buildingType;
};