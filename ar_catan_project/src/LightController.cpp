#include "LightController.h"#
#include <iostream>


LightController* LightController::inst_ = NULL;

LightController* LightController::getInstance()
{
	if (inst_ == NULL) {
		inst_ = new LightController();
	}
	return (inst_);
}

std::vector<LightSource*> LightController::getLightSources()
{
	if (this->m_lightSourceCount>0) {

		std::vector<LightSource*> sources;
		
		for (std::map<int, LightSource*>::iterator it = this->m_lightSources.begin(); it != this->m_lightSources.end(); ++it) {
			sources.push_back(it->second);
		}

		return sources;
	}
	return std::vector<LightSource*>();
}

LightSource LightController::getLightSource(int index)
{
	if(containsLightSource(index))
		return *this->m_lightSources[index];
	
}

int LightController::addLightSource(glm::vec3 position, glm::vec4 color, bool hidden)
{
	LightSource* l = new LightSource;
	l->position = position;
	l->color = color;
	l->hidden = hidden;
	l->directional = false;

	this->m_highestIndex++;
	this->m_lightSources.insert(std::make_pair(this->m_highestIndex,l));

	this->m_lightSourceCount++;

	return this->m_highestIndex;
}

int LightController::addDirectionalLightSource(glm::vec3 direction, glm::vec4 color, bool hidden)
{
	LightSource* l = new LightSource;
	l->direction = direction;
	l->color = color;
	l->hidden = hidden;
	l->directional = true;

	this->m_highestIndex++;
	this->m_lightSources.insert(std::make_pair(this->m_highestIndex, l));

	this->m_lightSourceCount++;

	return this->m_highestIndex;
}

void LightController::removeLightSource(int index)
{
	if (containsLightSource(index)) {
		this->m_lightSources.erase(index);
		this->m_lightSourceCount--;
	}
}

void LightController::setPosition(glm::vec3 position, int index)
{
	if (containsLightSource(index)) {
		if (this->m_lightSources[index]->directional) {
			this->m_lightSources[index]->direction = position;
		}
		else {
			this->m_lightSources[index]->position = position;
		}
	}
}

void LightController::setColor(glm::vec4 color, int index)
{
	if (containsLightSource(index)) {
		this->m_lightSources[index]->color = color;
	}
}

void LightController::setLightHidden(bool state, int index)
{
	if (containsLightSource(index)) {
		this->m_lightSources[index]->hidden = state;
	}
}

glm::vec3 LightController::getAmbientLight()
{
	return this->m_ambientColor * this->m_ambientStrength;
}

void LightController::setAmbientLight(glm::vec3 ambientColor, float ambientStrength)
{
	this->m_ambientColor = ambientColor;
	this->m_ambientStrength = ambientStrength;
}

bool LightController::containsLightSource(int index)
{
	std::map<int, LightSource*>::iterator itr;
	itr = this->m_lightSources.find(index);
	return itr != this->m_lightSources.end();
}
