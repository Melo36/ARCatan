#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "marker_tracker/MarkerTracker_PnP.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexBufferLayout.h"
#include "CameraController.h"

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <memory>

class CV_Content {
public:
	CV_Content();
	~CV_Content();

	std::pair<int, int> getFrameSize();

	void drawBackgroundImage(int winWidth, int winHeight);
	
	std::pair<vector <int>,vector<glm::mat4>> getMarkers();

	void initBackgroundRendering(GLFWwindow* window,CameraController* cameraController);

	void terminateBackgroundRendering();
	
private:
	glm::mat4 setUpCoordSys(cv::Mat ocv_Pmat);

	void initVideoStream();

	vector <int> MarkerID;
	vector<Mat> MarkerPmat;

	VideoCapture cap;

	std::unique_ptr <MarkerTracker> markerTracker;

	std::unique_ptr<VertexArray> m_vao;
	std::unique_ptr<VertexBuffer> m_vertexBuffer;
	std::unique_ptr<IndexBuffer> m_indexBuffer;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr <Texture> m_texture;
	glm::mat4 m_model;

	CameraController* m_cameraController;
	int m_orthoCam;
	Renderer* m_renderer;

	const double kMarkerSize = 0.0365; //0.0315 // 0.0355

	cv::Mat m_currentFrame;
};
