#include "Marker.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

GOMarker::GOMarker(int markerID, std::vector < std::shared_ptr<Mesh>> numberMesh, glm::mat4 numberOffset, std::vector<std::shared_ptr<Mesh>> meshes, std::shared_ptr<Shader> s, std::shared_ptr<Renderer> renderer, std::vector<Material> overrideMaterials)
	:GameObject(meshes, s, renderer, overrideMaterials), m_markerID(markerID),m_numberMesh(numberMesh),m_numberOffset(numberOffset)
{
	this->setScale(glm::vec3(0.00122));
	//p = glm::vec3(0.0);
}

GOMarker::GOMarker(int markerID, std::vector < std::shared_ptr<Mesh>> numberMesh, glm::mat4 numberOffset, std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> s, std::shared_ptr<Renderer> renderer, Material* overrideMaterial)
	:GameObject(mesh, s, renderer, overrideMaterial), m_markerID(markerID), m_numberMesh(numberMesh), m_numberOffset(numberOffset)
{
}

int GOMarker::getMarkerID()
{
	return m_markerID;
}

void GOMarker::onRender()
{
	
	GameObject::onRender();
	
	if (!m_hidden) {
		
		glm::mat4 model = m_model * m_numberOffset;//glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(p.x, p.y, p.z)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 mvp = m_cameraController->getViewProj() * glm::scale(model,m_scale);
		glm::mat4 normalMat = glm::transpose(glm::inverse(model)); // glm::scale(model, m_scale)
		

		m_shader->setUniformMat4f("u_MVP", mvp);
		m_shader->setUniformMat4f("u_model", glm::scale(model, m_scale));
		m_shader->setUniformMat4f("u_normalMat", normalMat);
		
		for (int i = 0; i < m_numberMesh.size(); i++) {
			m_numberMesh[i]->draw(GameObject::m_shader, GameObject::m_renderer.get());
		}
	}
}

void GOMarker::onImGuiRender()
{
	/*
		if (m_markerID == 0x1d59) {
		ImGui::Begin("Slider");
		ImGui::SliderFloat3("Pos", &p[0], -0.02, 0.02);
		ImGui::End();
		//std::cout << "x: " << p.x << " y: " << p.y << " z: " << p.z << std::endl;
	}*/
}
