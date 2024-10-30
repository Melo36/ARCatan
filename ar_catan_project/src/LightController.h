#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <map>

struct LightSource {
	glm::vec3 position;
	glm::vec3 direction;
	bool directional;
	bool hidden;
	glm::vec4 color;
};

class LightController {
public:

	static LightController* getInstance();

	LightController(LightController const&) = delete;
	void operator=(LightController const&) = delete;

	std::vector<LightSource*> getLightSources();

	LightSource getLightSource(int index);

	int addLightSource(glm::vec3 position, glm::vec4 color, bool hidden = false);

	int addDirectionalLightSource(glm::vec3 direction, glm::vec4 color, bool hidden = false);

	void removeLightSource(int index);

	void setPosition(glm::vec3 position, int index);

	void setColor(glm::vec4 color, int index);

	void setLightHidden(bool state, int index);

	glm::vec3 getAmbientLight();

	void setAmbientLight(glm::vec3 ambientColor, float ambientStrength);

private:
	static LightController* inst_;   // The one, single instance

	LightController() : m_lightSourceCount(0), m_highestIndex(0), m_ambientStrength(0.2f), m_ambientColor(glm::vec3(1.0)) {};

	~LightController();

	bool containsLightSource(int index);

	int m_highestIndex;

	int m_lightSourceCount;

	std::map<int, LightSource*> m_lightSources;
	 
	float m_ambientStrength;
	glm::vec3 m_ambientColor;
};