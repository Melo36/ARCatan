#pragma once

#include "GameObject.h"

class GOMarker : public GameObject {
public:
	GOMarker(int markerID, std::vector <std::shared_ptr<Mesh>>numberMesh, glm::mat4 numberOffset, std::vector<std::shared_ptr<Mesh>> meshes, std::shared_ptr <Shader>s, std::shared_ptr<Renderer> renderer, std::vector<Material> overrideMaterials = std::vector<Material>());
	GOMarker(int markerID, std::vector <std::shared_ptr<Mesh>> numberMesh, glm::mat4 numberOffset, std::shared_ptr<Mesh> mesh, std::shared_ptr <Shader>s, std::shared_ptr<Renderer> renderer, Material* overrideMaterial = nullptr);
	int getMarkerID();

	void onRender() override;
	void onImGuiRender() override;
private:
	//glm::vec3 p;
	int m_markerID;
	std::vector < std::shared_ptr<Mesh>> m_numberMesh;
	glm::mat4 m_numberOffset;
};