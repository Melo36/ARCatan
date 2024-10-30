#include "GameObject.h"
#include <iostream>

GameObject::GameObject(std::vector<std::shared_ptr<Mesh>> meshes, std::shared_ptr <Shader>s, std::shared_ptr<Renderer> renderer, std::vector<Material> overrideMaterials)
	: m_renderer(renderer),m_scale(glm::vec3(1.0)),m_translation(glm::vec3(0.0))
{
	m_cameraController = CameraController::getInstance();
	m_lightController = LightController::getInstance();
	m_meshes = meshes;

	m_shader = s;

	m_hidden = true;
	m_model = glm::mat4(1.0f);

	if (!overrideMaterials.empty()) {
		m_overrideMeshMaterials = true;
		m_overrideMaterials = overrideMaterials;
	}

}

GameObject::GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> s, std::shared_ptr<Renderer> renderer, Material* overrideMaterial)
	: m_renderer(renderer), m_scale(glm::vec3(1.0)), m_translation(glm::vec3(0.0))
{
	m_cameraController = CameraController::getInstance();
	m_lightController = LightController::getInstance();
	m_meshes = std::vector<std::shared_ptr<Mesh>>{ mesh };

	m_shader = s;

	m_hidden = true;
	//m_color = glm::vec4(1.0, 1.0, 1.0, 1.0);
	m_model = glm::mat4(1.0f);

	if (overrideMaterial != nullptr) {
		m_overrideMeshMaterials = true;
		m_overrideMaterials = std::vector<Material>{ *overrideMaterial};
	}
}

GameObject::~GameObject()
{
	
}
//m_markerID(other.m_markerID),m_color(other.m_color),
GameObject::GameObject(const GameObject& other)
	:m_meshes(other.m_meshes), m_shader(other.m_shader), m_overrideMaterials(other.m_overrideMaterials),m_overrideMeshMaterials(other.m_overrideMeshMaterials),m_scale(other.m_scale),
	m_translation(glm::vec3(0.0)),m_cameraController(other.m_cameraController),m_lightController(other.m_lightController), m_renderer(other.m_renderer), m_model(other.m_model),
	m_hidden(other.m_hidden)
{

}

void GameObject::onUpdate(float deltaTime)
{
}

void GameObject::onRender()
{
	if (!m_hidden) {
		glm::mat4 mvp = m_cameraController->getViewProj() * glm::translate(glm::scale(m_model,m_scale),m_translation);
		glm::mat4 normalMat = glm::transpose(glm::inverse(m_model)); //glm::scale(m_model,m_scale)

		//set shader uniforms
		m_shader->bind();
		m_shader->setUniformMat4f("u_MVP", mvp);
		m_shader->setUniformMat4f("u_model", glm::translate(glm::scale(m_model,m_scale),m_translation));
		m_shader->setUniformMat4f("u_normalMat", normalMat);

		//set uniforms for lighting
		glm::vec3 viewPos = glm::inverse(m_cameraController->getView())[3];
		m_shader->setUniform3f("view_pos", viewPos);
		std::vector<LightSource*> lights = m_lightController->getLightSources();
		glm::vec3 ambientLight = m_lightController->getAmbientLight();

		for (int i = 0; i < lights.size(); i++) {
			LightSource* light = lights[i];
			std::string number = std::to_string(i);
			//std::cout << glm::to_string(light->position) << std::endl;
			if (!light->hidden) {
				if (light->directional) {
					m_shader->setUniform4f("lights[" + number + "].vector", light->direction.x, light->direction.y, light->direction.z, 0.0);
				}
				else {
					m_shader->setUniform4f("lights[" + number + "].vector", light->position.x, light->position.y, light->position.z, 1.0);
				}
				m_shader->setUniform3f("lights[" + number +"].color", light->color);
			}
			m_shader->setUniform3f("lights[" + number + "].ambient", ambientLight);
		}

		for (int i = 0; i < m_meshes.size(); i++) {
			//mesh
			if (m_overrideMeshMaterials) {
				m_meshes[i]->draw(m_shader, m_renderer.get(), &m_overrideMaterials[i]);
			}
			else {
				m_meshes[i]->draw(m_shader, m_renderer.get());
			}
		}
	}
}

void GameObject::onImGuiRender()
{
	/*
	ImGui::SliderFloat3("TranslationA", &translationA.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("TranslationB", &translationB.x, -100.0f, 100.0f);
	*/

}

void GameObject::setModelMatrix(glm::mat4 model)
{
	m_model = model;
}

glm::mat4 GameObject::getModelMatrix()
{
	return m_model;
}

void GameObject::setHidden(bool state)
{
	m_hidden = state;
}

void GameObject::setScale(glm::vec3 scale)
{
	m_scale = scale;
}

void GameObject::setTranslation(glm::vec3 translation)
{
	m_translation = translation;
}

bool GameObject::isHidden()
{
	return m_hidden;
}
