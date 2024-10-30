#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <map>

struct Camera {
	glm::mat4 view;
	glm::mat4 proj;
};

struct CameraOrtho: public Camera {
	float left;
	float right;
	float bottom;
	float top;
	float nearZ;
	float farZ;
};

struct CameraPerspective : public Camera {
	float fovy;
	float aspect;
	float nearZ;
	float farZ;
};

class CameraController {
public:

	static CameraController* getInstance();

	CameraController(CameraController const&) = delete;
	void operator=(CameraController const&) = delete;


	int addCamera(glm::mat4 view, glm::mat4 perspectiveProj);
	
	int addPerpectiveCamera(glm::mat4 view, float fovy, float aspect, float nearZ, float farZ);

	int addOrthoCamera(glm::mat4 view, float left, float right, float bottom, float top, float nearZ, float farZ);

	void setProjection(glm::mat4 projMat, int camIndex = -1);

	void setPerspectiveProjection(float fovy, float aspect, float nearZ, float farZ, int camIndex = -1);

	void setOrthoProjection(float left, float right, float bottom , float top, float nearZ, float farZ, int camIndex = -1);

	void setView(glm::mat4 view, int camIndex = -1);

	void setActiveCam(int camIndex);

	int getActiveCam();

	void removeCamera(int camIndex);

	glm::mat4 getViewProj(int camIndex = -1);

	glm::mat4 getView(int camIndex = -1);

	glm::mat4 getProj(int camIndex = -1);


private:

	static CameraController* inst_;   // The one, single instance

	CameraController() : activeCamIndex(-1), cameraCount(0), highestIndex(0) {};

	~CameraController();

	bool containsCameraIndex(int index);

	int activeCamIndex;

	int highestIndex;

	int cameraCount;

	std::map<int,Camera*> cameras;
};