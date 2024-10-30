#include "CameraController.h"


CameraController* CameraController::inst_ = NULL;

CameraController* CameraController::getInstance()
{
	if (inst_ == NULL) {
		inst_ = new CameraController();
	}
	return (inst_);
}

int CameraController::addCamera(glm::mat4 view, glm::mat4 perspectiveProj)
{
	Camera* cam = new Camera;
	cam->proj = perspectiveProj;
	cam->view = view;

	this->highestIndex++;
	this->cameras.insert(std::make_pair(this->highestIndex, cam));
	this->cameraCount++;

	if (this->activeCamIndex == -1)
		this->activeCamIndex = highestIndex;

	return this->highestIndex;
}

int CameraController::addPerpectiveCamera(glm::mat4 view, float fovy, float aspect, float nearZ, float farZ)
{
	CameraPerspective* cam = new CameraPerspective;
	cam->proj = glm::perspective(fovy,aspect,nearZ,farZ);
	cam->view = view;
	cam->aspect = aspect;
	cam->farZ = farZ;
	cam->nearZ = nearZ;
	cam->fovy = fovy;

	this->highestIndex++;
	this->cameras.insert(std::make_pair(this->highestIndex, cam));
	this->cameraCount++;

	if (this->activeCamIndex == -1)
		this->activeCamIndex = highestIndex;

	return this->highestIndex;
}

int CameraController::addOrthoCamera(glm::mat4 view, float left, float right, float bottom, float top, float nearZ, float farZ)
{
	CameraOrtho* cam = new CameraOrtho;
	cam->proj = glm::ortho(left,right,bottom,top,nearZ,farZ);
	cam->view = view;
	cam->left = left;
	cam->right = right;
	cam->bottom = bottom;
	cam->nearZ = nearZ;
	cam->farZ = farZ;


	this->highestIndex++;
	this->cameras.insert(std::make_pair(this->highestIndex,cam));
	this->cameraCount++;

	if (this->activeCamIndex == -1)
		this->activeCamIndex = highestIndex;

	return this->highestIndex;
}

void CameraController::setProjection(glm::mat4 projMat, int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)){

		Camera* newCam = new Camera;
		newCam->proj = projMat;
		newCam->view = this->cameras[camIndex]->view;
		
		this->cameras[camIndex] = newCam;
	}
}

void CameraController::setPerspectiveProjection(float fovy, float aspect, float nearZ, float farZ, int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)) {
		CameraPerspective* cam = new CameraPerspective;
		cam->proj = glm::perspective(fovy, aspect, nearZ, farZ);
		cam->view = this->cameras[camIndex]->view;
		cam->aspect = aspect;
		cam->farZ = farZ;
		cam->nearZ = nearZ;
		cam->fovy = fovy;

		this->cameras[camIndex] = cam;
	}
}

void CameraController::setOrthoProjection(float left, float right, float bottom, float top, float nearZ, float farZ, int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)) {
		CameraOrtho* cam = new CameraOrtho;
		cam->proj = glm::ortho(left, right, bottom, top, nearZ, farZ);
		cam->view = this->cameras[camIndex]->view;
		cam->left = left;
		cam->right = right;
		cam->bottom = bottom;
		cam->nearZ = nearZ;
		cam->farZ = farZ;
		
		this->cameras[camIndex] = cam;
	}
}

void CameraController::setView(glm::mat4 view, int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)) {
		this->cameras[camIndex]->view = view;
	}
}

void CameraController::setActiveCam(int camIndex)
{
	if(containsCameraIndex(camIndex))
		activeCamIndex = camIndex;
}

int CameraController::getActiveCam()
{
	return activeCamIndex;
}

void CameraController::removeCamera(int camIndex)
{
	if (containsCameraIndex(camIndex)) {
		this->cameras.erase(camIndex);
		this->cameraCount--;
		if (this->activeCamIndex == camIndex) {
			this->activeCamIndex = -1;
		}
	}
}

glm::mat4 CameraController::getViewProj(int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)) {
		return (this->cameras[camIndex]->proj * this->cameras[camIndex]->view);
	}
	return glm::mat4();
}

glm::mat4 CameraController::getView(int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)) {
		return this->cameras[camIndex]->view;
	}
	return glm::mat4();
}

glm::mat4 CameraController::getProj(int camIndex)
{
	if (camIndex == -1)
		camIndex = activeCamIndex;

	if (containsCameraIndex(camIndex)) {
		return this->cameras[camIndex]->proj;
	}
	return glm::mat4();
}

CameraController::~CameraController()
{
	//delete all cameras
	std::map<int, Camera*>::iterator itr;
	for (itr = cameras.begin(); itr != cameras.end(); ++itr) {
		cameras.erase(itr->first);
	}

}

bool CameraController::containsCameraIndex(int index)
{
	std::map<int, Camera*>::iterator itr;
	itr = this->cameras.find(index);
	return itr != this->cameras.end();
	
}
