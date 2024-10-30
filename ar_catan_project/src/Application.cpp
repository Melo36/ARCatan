#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "ObjectLoader.h"
#include "CameraController.h"
#include "game_objects/GameObject.h"
#include "game_objects/Building.h"
#include "game_objects/Marker.h"
#include "CV_Content.h"
#include "LightController.h"
#include "Material.h"

#include "marker_tracker/ogl_UI.h"
#include "game_logic/GameController.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define G_WIDTH 16
#define G_HEIGHT 9
int width;
int height;

//--------------
// setup GAMELOGIC
//--------------
GameController gameController = GameController();

using namespace std;

//create GameObjects. Initially one for each kind of marker
std::vector<std::shared_ptr<Texture> > ui_textures;
std::vector<std::shared_ptr<GameObject> > gameObjects;
std::vector<std::shared_ptr<GOMarker> > go_markers;
std::vector<std::shared_ptr<GOBuilding> > go_buildings;

std::shared_ptr<GOBuilding> defaultSettlement;
std::shared_ptr<GOBuilding> defaultRoad;
std::shared_ptr<GOBuilding> defaultCity;

//---------------------------------------------------------------------------
void reshape(GLFWwindow *window, int in_width, int in_height)
{
	//---------------------------------------------------------------------------
	// Set a whole-window viewport

	width = in_width;
	height = in_height;
	glViewport(0, 0, (GLsizei)in_width, (GLsizei)in_height);

	float near = 0.1f;
	float far = 500.0f;
	int max_d = cv::max(in_width, in_height);
	float fx = max_d;
	float fy = max_d;
	float cx = (float)in_width / 2.0f;
	float cy = (float)in_height / 2.0f;

	float A = -(far + near) / (far - near);
	float B = -(2 * far * near) / (far - near);
	glm::mat4 projt = {fx / cx, 0.0f, 0.0f, 0.0f,
					   0.0f, fy / cy, 0.0f, 0.0f,
					   0.0f, 0.0f, A, B,
					   0.0f, 0.0f, -1.0f, 0.0f};

	CameraController::getInstance()->setProjection(glm::transpose(projt));

	/*
	// Calculate the proper aspect ratio to use based on window ratio
	float ratioX = width / (float)G_WIDTH;
	float ratioY = height / (float)G_HEIGHT;
	float ratio = ratioX < ratioY ? ratioX : ratioY;
	// Calculate the width and height that the will be rendered to
	float viewWidth = (int) (G_WIDTH * ratio);
	float viewHeight = (int) (G_HEIGHT * ratio);
	// Calculate the position, which will apply proper "pillar" or "letterbox"
	float viewX = (int) ((width - G_WIDTH * ratio) / 2);
	float viewY = (int)((height - G_HEIGHT * ratio) / 2);
	// Apply the viewport and switch back to the GL_MODELVIEW matrix mode
	glViewport(viewX, viewY, viewWidth, viewHeight);*/
}

void initGL(int argc, char *argv[])
{

	glClearColor(0, 0, 0, 1.0);

	// Enable and set depth parameters
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	//texture Blending
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

glm::mat4 createProj(int in_width, int in_height)
{
	float near = 0.1f;
	float far = 500.0f;
	int max_d = cv::max(in_width, in_height);
	float fx = max_d;
	float fy = max_d;
	float cx = (float)in_width / 2.0f;
	float cy = (float)in_height / 2.0f;

	float A = -(far + near) / (far - near);
	float B = -(2 * far * near) / (far - near);
	glm::mat4 projt = {fx / cx, 0.0f, 0.0f, 0.0f,
					   0.0f, fy / cy, 0.0f, 0.0f,
					   0.0f, 0.0f, A, B,
					   0.0f, 0.0f, -1.0f, 0.0f};
	return glm::transpose(projt);
}

std::shared_ptr<Renderer> loadAssets()
{

	//load objects
	ObjectLoader *objL = ObjectLoader::getInstance();
	//std::vector<Mesh> meshesObj1 = objL->loadObjectFromFile("res/objects/monkey.obj", Meshtype::PN);
	//Mesh cubeMesh = objL->loadCubeVTN();

	std::vector<std::shared_ptr<Mesh>> hillsMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/brick.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> desertMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/desert.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> harbourMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/hafen.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> mountainsMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/ore.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> oceanMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/water.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> forestMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/wood.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> pasturesMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/wool.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> fieldsMesh = objL->loadObjectFromFile("../assets/assets_stl/felder/wheat.obj", Meshtype::PNC);

	std::vector<std::shared_ptr<Mesh>> settlementMesh = objL->loadObjectFromFile("../assets/assets_stl/buildings/haus.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> cityMesh = objL->loadObjectFromFile("../assets/assets_stl/buildings/stadt.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> roadMesh = objL->loadObjectFromFile("../assets/assets_stl/buildings/strasse.obj", Meshtype::PNC);

	std::vector<std::shared_ptr<Mesh>> number2Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/2_1x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number3Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/3_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number4Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/4_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number5Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/5_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number6Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/6_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number8Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/8_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number9Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/9_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number10Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/10_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number11Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/11_2x.obj", Meshtype::PNC);
	std::vector<std::shared_ptr<Mesh>> number12Mesh = objL->loadObjectFromFile("../assets/assets_stl/numbers_mit anzahl/12_1x.obj", Meshtype::PNC);

	std::vector<std::shared_ptr<Mesh>> nextTurnMesh = objL->loadObjectFromFile("../assets/Arrow.obj",Meshtype::PNC);
	objL->~ObjectLoader();

	//load shaders
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("res/shaders/lightingMaterial.shader");
	std::shared_ptr<Shader> buildingShader = std::make_shared<Shader>("res/shaders/lightingMaterialColor.shader");

	//load textures
	//std::shared_ptr<Texture> containerDiffuseTexture = std::make_shared<Texture>("res/textures/container_d.png");
	//std::shared_ptr<Texture> containerSpecularTexture = std::make_shared<Texture>("res/textures/container_s.png");
	std::shared_ptr<Texture> ui_inventory = std::make_shared<Texture>("../assets/on_screen/inventory.png");
	std::shared_ptr<Texture> ui_dice = std::make_shared<Texture>("../assets/on_screen/dice.png");
	std::shared_ptr<Texture> ui_crafting = std::make_shared<Texture>("../assets/on_screen/crafting.png");
	std::shared_ptr<Texture> ui_score = std::make_shared<Texture>("../assets/on_screen/siegespunkt.png");

	ui_textures.push_back(ui_inventory);
	ui_textures.push_back(ui_dice);
	ui_textures.push_back(ui_crafting);
	ui_textures.push_back(ui_score);

	//create materials
	//Material gold (glm::vec4(0.24725,0.1995,0.0745,1.0),glm::vec4(0.75164,0.60648,0.22648,1.0),glm::vec4(0.628281,0.555802,0.366065,1.0),0.1);
	//Material box(containerDiffuseTexture, containerSpecularTexture, 10.0,shader);

	//create renderer
	std::shared_ptr<Renderer> renderer;

	glm::mat4 forestNumberOffset = glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(-0.015, 0.01, 0.016)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 hillsNumberOffset = glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(-0.018,0.007,-0.005)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 mountainNumberOffset = glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(0.006,0.011,0.02)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 pasturesNumberOffset = glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(0.0195,0.012,-0.00225)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	glm::mat4 fieldsNumberOffset = glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(0.01,0.012,0.016)), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	
	//create gameobjects for each tile marker
	for (Tile tile : gameController.terrain) {
		std::vector<std::shared_ptr<Mesh>> numberMesh;
		switch (tile.diceNumber)
		{
		case 2:
			numberMesh = number2Mesh;
			break;
		case 3:
			numberMesh = number3Mesh;
			break;
		case 4:
			numberMesh = number4Mesh;
			break;
		case 5:
			numberMesh = number5Mesh;
			break;
		case 6:
			numberMesh = number6Mesh;
			break;
		case 8:
			numberMesh = number8Mesh;
			break;
		case 9:
			numberMesh = number9Mesh;
			break;
		case 10:
			numberMesh = number10Mesh;
			break;
		case 11:
			numberMesh = number11Mesh;
			break;
		case 12:
			numberMesh = number12Mesh;
			break;
		default:
			numberMesh = std::vector<std::shared_ptr<Mesh>>{std::make_shared<Mesh>()};
			break;
		}

		std::vector<std::shared_ptr<Mesh>> tileMesh;
		glm::mat4 numberOffset;
		switch (tile.type)
		{
		case hills:
			tileMesh = hillsMesh;
			numberOffset = hillsNumberOffset;
			break;
		case forest:
			tileMesh = forestMesh;
			numberOffset = forestNumberOffset;
			break;
		case mountains:
			tileMesh = mountainsMesh;
			numberOffset = mountainNumberOffset;
			break;
		case fields:
			tileMesh = fieldsMesh;
			numberOffset = fieldsNumberOffset;
			break;
		case pastures:
			tileMesh = pasturesMesh;
			numberOffset = pasturesNumberOffset;
			break;
		case desert:
			tileMesh = desertMesh;
			numberOffset = glm::mat4(1.0);
			break;
		default:
			tileMesh = oceanMesh;
			numberOffset = glm::mat4(1.0);
			break;
		}

		//std::cout << tile.MarkerID << std::endl;
		std::shared_ptr<GOMarker> tileObject = std::make_shared<GOMarker>(tile.MarkerID, numberMesh, numberOffset, tileMesh, shader, renderer);
		gameObjects.push_back(tileObject);
		go_markers.push_back(tileObject);
	}
	
	//create building markers
	for (map<int, BuildingType>::iterator it = gameController.placeBuildingMarkers.begin(); it != gameController.placeBuildingMarkers.end(); it++) {
		std::vector<std::shared_ptr<Mesh>> buildingMesh;
		switch (it->second) {
		case Settlement:
			buildingMesh = settlementMesh;
			break;
		case Road:
			buildingMesh = roadMesh;
			break;
		case City:
			buildingMesh = cityMesh;
			break;
		}
		std::shared_ptr<GOMarker> buildingMarker = std::make_shared<GOMarker>(it->first, std::vector<std::shared_ptr<Mesh>>(), glm::mat4(1.0), buildingMesh, buildingShader, renderer);
		buildingMarker->setTranslation(glm::vec3(0.0,15,0.0));
		buildingMarker->setScale(glm::vec3(0.0008));
		gameObjects.push_back(buildingMarker);
		go_markers.push_back(buildingMarker);
	}

	//create next turn marker
	std::shared_ptr<GOMarker> nextTurnMarker = std::make_shared<GOMarker>(gameController.nextTurnMarker, std::vector<std::shared_ptr<Mesh>>(), glm::mat4(1.0), nextTurnMesh, shader, renderer);
	nextTurnMarker->setTranslation(glm::vec3(0.0, 200, 0.0));
	nextTurnMarker->setScale(glm::vec3(0.00066));
	gameObjects.push_back(nextTurnMarker);
	go_markers.push_back(nextTurnMarker);

	defaultSettlement = std::make_shared<GOBuilding>(glm::vec4(1.0, 1.0, 0.0, 1.0), settlementMesh, buildingShader, renderer);
	defaultRoad = std::make_shared<GOBuilding>(glm::vec4(1.0, 1.0, 0.0, 1.0), roadMesh, buildingShader, renderer);
	defaultCity = std::make_shared<GOBuilding>(glm::vec4(1.0, 1.0, 0.0, 1.0), cityMesh, buildingShader, renderer);
	//go_buildings.push_back(b);
	//gameObjects.push_back(b);
	/*
	std::shared_ptr<GOBuilding> b1 = std::make_shared<GOBuilding>(glm::vec4(1.0, 1.0, 0.0, 1.0), settlementMesh, buildingShader, renderer);
	b1->setModelMatrix(glm::translate(glm::mat4(1.0),glm::vec3(-0.042,0,0.24)));
	b1->setHidden(false);
	gameObjects.push_back(b1);*/

	return renderer;
}

vec4 projectToWorldPlane(vec3 absBuildingTrans) {
	//glm::vec3 absBuildingTrans = glm::vec3(relativeToWorldReference[3]);

	//Ray
	glm::vec3 Ray_ori = glm::inverse(CameraController::getInstance()->getView())[3]; //CameraPosition
	glm::vec3 Ray_dir = glm::normalize(absBuildingTrans - Ray_ori);
	float t;
	//Plane
	glm::vec3 Plane_norm = glm::vec3(0, 1, 0);
	glm::vec3 Plane_ori = glm::vec3(0, 0.0, 0); //testing height ob buildings above tiles

	// assuming vectors are all normalized
	float denom = glm::dot(Plane_norm, Ray_dir);
	//std::cout << denom << std::endl;
	glm::vec3 p0l0 = Plane_ori - Ray_ori;
	t = dot(p0l0, Plane_norm) / denom;

	glm::vec3 newBuildingPlacement = Ray_ori + t * Ray_dir;;
	return glm::vec4(newBuildingPlacement, 1.0);
}

void updateMarkerPoses(std::vector<int> markerIDs, vector<glm::mat4> markerMats)
{
	//set model matrices
	for (std::shared_ptr<GOMarker> gameObject : go_markers)
	{
		int markerId = gameObject->getMarkerID();
		bool match = false;
		for (size_t i = 0; i < markerIDs.size(); i++)
		{
			if (markerId == markerIDs[i])
			{
				gameObject->setModelMatrix(markerMats[i]);
				gameObject->setHidden(false);
				match = true;
				break;
			}
		}

		//hide building markers only
		if (gameController.placeBuildingMarkers.count(markerId) != 0 || gameController.nextTurnMarker == markerId) {
			//hide gameObjects with no matched marker for now
			gameObject->setHidden(!match);
		}
	}
}

void renderUI(int *resourceCount, int diceNumber, int score, int playerid, glm::vec4 color)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//std::cout << min_d << std::endl;
	

	//inventory
	int inventoryTextureWidth = ui_textures[0]->getWidth();
	int inventoryTextureHeight = ui_textures[0]->getHeight();
	float inventoryTextureAspectRatio = inventoryTextureHeight / (float)inventoryTextureWidth;
	int inventoryWidth = width * 0.5;
	int inventoryHeight = inventoryWidth * inventoryTextureAspectRatio;
	float inventoryScaleWidth = inventoryWidth / (float)inventoryTextureWidth;
	float inventoryScaleHeight = inventoryHeight / (float)inventoryTextureHeight;
	ImGui::SetNextWindowPos(ImVec2((width / 2) - (inventoryWidth / 2), height - inventoryHeight));
	ImGui::SetNextWindowSize(ImVec2(inventoryWidth, inventoryHeight));
	ImGui::Begin("Inventory", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);

	ImGui::Image((void *)(intptr_t)ui_textures[0]->getRendererID(), ImVec2(inventoryTextureWidth * inventoryScaleWidth, inventoryTextureHeight * inventoryScaleHeight));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	//ImGui::GetFont()->Scale = width / (float)1800;
	//ImGui::PushFont(ImGui::GetFont());
	//ImGui::SetWindowFontScale(width / (float)1800);
	ImGui::SetWindowFontScale(inventoryScaleWidth * 10.0f);

	std::string s;
	float textWidth;

	//wood
	s = std::to_string(resourceCount[1]);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((inventoryTextureWidth * inventoryScaleWidth * 0.2825) - (textWidth * 0.5)), inventoryTextureHeight * inventoryScaleHeight * 0.75));
	ImGui::Text(s.c_str());

	//brick
	s = std::to_string(resourceCount[0]);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((inventoryTextureWidth * inventoryScaleWidth * 0.3925) - (textWidth * 0.5)), inventoryTextureHeight * inventoryScaleHeight * 0.75));
	ImGui::Text(s.c_str());

	//wool
	s = std::to_string(resourceCount[4]);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((inventoryTextureWidth * inventoryScaleWidth * 0.5025) - (textWidth * 0.5)), inventoryTextureHeight * inventoryScaleHeight * 0.75));
	ImGui::Text(s.c_str());

	//wheat
	s = std::to_string(resourceCount[3]);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((inventoryTextureWidth * inventoryScaleWidth * 0.6125) - (textWidth * 0.5)), inventoryTextureHeight * inventoryScaleHeight * 0.75));
	ImGui::Text(s.c_str());

	//stone
	s = std::to_string(resourceCount[2]);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((inventoryTextureWidth * inventoryScaleWidth * 0.7225) - (textWidth * 0.5)), inventoryTextureHeight * inventoryScaleHeight * 0.75));
	ImGui::Text(s.c_str());

	ImGui::PopStyleColor();
	//ImGui::PopFont();
	ImGui::End();

	//crafting
	int craftingTextureWidth = ui_textures[2]->getWidth();
	int craftingTextureHeight = ui_textures[2]->getHeight();
	float craftingTextureAspectRatio = craftingTextureHeight / (float)craftingTextureWidth;
	int craftingWidth = width * 0.2;
	int craftingHeight = craftingWidth * craftingTextureAspectRatio;
	float craftingScaleWidth = craftingWidth / (float)craftingTextureWidth;
	float craftingScaleHeight = craftingHeight / (float)craftingTextureHeight;
	ImGui::SetNextWindowPos(ImVec2(width - craftingWidth, height - craftingHeight));
	ImGui::SetNextWindowSize(ImVec2(craftingWidth, craftingHeight));
	ImGui::Begin("Crafting", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	ImGui::Image((void *)(intptr_t)ui_textures[2]->getRendererID(), ImVec2(craftingTextureWidth * craftingScaleWidth - 12.5, craftingTextureHeight * craftingScaleHeight));
	ImGui::End();

	//dice
	int diceTextureWidth = ui_textures[1]->getWidth();
	int diceTextureHeight = ui_textures[1]->getHeight();
	float diceTextureAspectRatio = diceTextureHeight / (float)diceTextureWidth;
	int diceWidth = width * 0.125;
	int diceHeight = diceWidth * diceTextureAspectRatio;
	float diceScaleWidth = diceWidth / (float)diceTextureWidth;
	float diceScaleHeight = diceHeight / (float)diceTextureHeight;
	ImGui::SetNextWindowPos(ImVec2(-10, -10));
	ImGui::SetNextWindowSize(ImVec2(diceWidth, diceHeight));
	ImGui::Begin("Dice", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	ImGui::Image((void *)(intptr_t)ui_textures[1]->getRendererID(), ImVec2(diceTextureWidth * diceScaleWidth, diceTextureHeight * diceScaleHeight));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	ImGui::SetWindowFontScale(diceScaleWidth * 10.0f);
	//dice number
	s = std::to_string(diceNumber);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((diceWidth * 0.2825) - (textWidth * 0.5)), diceHeight * 0.25));
	ImGui::Text(s.c_str());

	ImGui::PopStyleColor();
	ImGui::End();

	//score
	int scoreTextureWidth = ui_textures[3]->getWidth();
	int scoreTextureHeight = ui_textures[3]->getHeight();
	float scoreTextureAspectRatio = scoreTextureHeight / (float)scoreTextureWidth;
	int scoreWidth = width * 0.125;
	int scoreHeight = scoreWidth * scoreTextureAspectRatio;
	float scoreScaleWidth = scoreWidth / (float)scoreTextureWidth;
	float scoreScaleHeight = scoreHeight / (float)scoreTextureHeight;
	ImGui::SetNextWindowPos(ImVec2(width-scoreWidth + 5, -10));
	ImGui::SetNextWindowSize(ImVec2(scoreWidth, scoreHeight));
	ImGui::Begin("Score", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	ImGui::Image((void*)(intptr_t)ui_textures[3]->getRendererID(), ImVec2(scoreTextureWidth * scoreScaleWidth, scoreTextureHeight * scoreScaleHeight));
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
	ImGui::SetWindowFontScale(scoreScaleWidth * 10.0f);

	//score number
	s = std::to_string(score);
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	ImGui::SetCursorPos(ImVec2(((scoreWidth * 0.8) - (textWidth * 0.5)), scoreHeight * 0.25));
	ImGui::Text(s.c_str());

	ImGui::PopStyleColor();
	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(textWidth, scoreHeight));
	
	int idWidth = width * 0.2;
	int idHeight = idWidth * 0.1;//* idAspectRatio;
	ImGui::SetNextWindowPos(ImVec2((width / 2.0) - (idWidth / 2.0), height - (2 *inventoryHeight / 3)));
	ImGui::SetNextWindowSize(ImVec2(idWidth, idHeight));
	ImGui::Begin("PlayerID", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	ImGui::SetWindowFontScale(idWidth * 0.003);

	s = ("Player " + std::to_string(playerid));
	textWidth = ImGui::CalcTextSize(s.c_str()).x;
	float textHeight = ImGui::CalcTextSize(s.c_str()).y;
	//float idAspectRatio = textHeight / (float)textWidth;
	//float idScaleWidth = idWidth / (float)textWidth;
	//float idScaleHeight = idHeight / (float)textHeight;

	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32( (int)(color.x * 255), (int)(color.y * 255), (int)(color.z * 255), (int)(color.w * 255)));
	ImGui::SetCursorPos(ImVec2((idWidth / 2) - (textWidth / 2), (idHeight / 2) - (textHeight / 2)));
	ImGui::Text(s.c_str());
	ImGui::PopStyleColor();
	ImGui::End();

	for (std::shared_ptr<GameObject> object : gameObjects) {
		object->onImGuiRender();
	}

	//finally render ImGUI windwos
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main(int argc, char *argv[])
{

	//-------------
	// Setup OpenCV
	//-------------

	CV_Content cv_content;
	std::pair<int, int> dimensions = cv_content.getFrameSize();
	//width = dimensions.first, height = dimensions.second;
	float cameraAspect =  dimensions.first / (float) dimensions.second;
	//std::cout << cameraAspect << std::endl;

	//-------------
	// Setup OpenGL
	//-------------

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char *glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char *glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
																   //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	GLFWwindow *window;

	if (!glfwInit())
		return -1;

	//get primary monitor width and height
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = mode->height * cameraAspect, height = mode->height;
	

	//at the moment windowed mode.Add glfwGetPrimaryMonitor() for full screen
	//                                                    \/
	window = glfwCreateWindow(width, height, "AR_Catan", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//glfwMaximizeWindow(window);
	//glfwFocusWindow(window);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	//-----------------
	// user interaction
	//-----------------
	UI_handler UI(width, height);

	glfwSetWindowUserPointer(window, &UI);
	UI.setCallbacks(window);
	glfwSetFramebufferSizeCallback(window, reshape);
	reshape(window, width, height);
	// Initialize the GL library
	initGL(argc, argv);

	// Initialize Glew with valid rendering context
	if (glewInit() != GLEW_OK)
		return -1;

	{

		//setup Lighting
		LightController *lightController = LightController::getInstance();
		int light0 = lightController->addDirectionalLightSource(glm::vec3(-1.0, -0.5, 0.0), glm::vec4(1.0, 1.0, 1.0, 1.0));
		//lightController->addLightSource(glm::vec3(0.1, 0.0, 0.0), glm::vec4(1.0, 1.0, 1.0, 1.0));//

		//create CameraController
		CameraController *cameraController = CameraController::getInstance();

		//add camera
		//glm::mat4 proj = glm::ortho(0.0f, 640.0f, 0.0f, 480.0f, 0.0f, 100.0f)
		//glm::mat4 proj = glm::perspective(45.0f, (width / (float)height), 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));
		int cam0 = cameraController->addCamera(view, createProj(width, height));

		//initialize background rendering
		cv_content.initBackgroundRendering(window, cameraController);

		//initialize Gamelogic
		gameController.initializeGame();

		//create renderer and load assets
		std::shared_ptr<Renderer> renderer = loadAssets();

		//setup ImGUI
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
		//setup font
		io.Fonts->AddFontFromFileTTF("res\\fonts\\PressStart2P-Regular.ttf", 30.0f);
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		//init time measurement
		float time = glfwGetTime();

		// am Anfang kann jeder abwechselnd Gebäude platzieren

		// TODO: get this Matrix from player interaction and convert it to tile and pos
		//int arr[16];
		//mat4 temp = make_mat4(arr);

		//Player 1 hat ID 0 und Player 2 hat ID 1
		/*
		gameController.placeSettlementAndStreetAtStartOfGame(0, temp);
		gameController.placeSettlementAndStreetAtStartOfGame(1, temp);
		gameController.placeSettlementAndStreetAtStartOfGame(0, temp);
		gameController.placeSettlementAndStreetAtStartOfGame(1, temp);
		*/

		bool worldReferenceLost = false;
		// Loop until the user closes the window
		while (UI.contLoop())
		{
			// Render here
			renderer->clear();
			
			//query playerstate from gamecontroller (resourcecount, color etc.)
			Player p = gameController.getActivePlayer();

			//query markers
			std::pair<vector<int>, vector<glm::mat4> > markers = cv_content.getMarkers();

			mat4 worldRef = gameController.getWorldReferenceMarker(markers.first, markers.second);

			//if no world reference marker around => disable dispaying all objects.
			if (worldRef == glm::mat4(1.0)) {
				worldReferenceLost = true;
				//hide all objects
				for (size_t j = 0; j < gameObjects.size(); j++) {
					gameObjects[j]->setHidden(true);
				}
			}
			else {

				if (worldReferenceLost) {
					//unhide buildings (markers are unhidden when their pose is updated)
					for (size_t j = 0; j < go_buildings.size(); j++) {
						go_buildings[j]->setHidden(false);
					}
					worldReferenceLost = false;
				}

				//set camera view and and light position to camera
				cameraController->setView(worldRef);
				if (lightController->getLightSource(light0).directional) {
					lightController->setPosition((-1.0f * glm::vec3(glm::inverse(worldRef)[3])), light0);
				}
				else {
					lightController->setPosition((glm::vec3(glm::inverse(worldRef)[3])), light0);
				}
				

				//change conversion relative to world
				for (int i = 0; i < markers.first.size(); i++)
				{
					markers.second[i] = glm::inverse(worldRef) * markers.second[i];

					
					//extract rotation part
					mat3 rotation = { markers.second[i][0][0], markers.second[i][0][1], markers.second[i][0][2],markers.second[i][1][0], markers.second[i][1][1], markers.second[i][1][2],markers.second[i][2][0], markers.second[i][2][1], markers.second[i][2][2] };
					glm::quat q = glm::quat_cast(rotation);
					
					//remove rotation angles in z and x (since all markers are expected to be in a plane only y rotation is needed)
					q[0] = 0;
					q[2] = 0;
					q = glm::normalize(q);
					mat3 newRot = glm::mat3_cast(q);

					//project position to world marker plane for better alignment
					glm::vec4 newT = projectToWorldPlane(markers.second[i][3]);
					
					//build new transformation matrix
					glm::mat4 newTransformation = newRot;
					newTransformation[3] = newT;
					markers.second[i] = newTransformation;
					
				}

				updateMarkerPoses(markers.first, markers.second);

				//render buildings

				//update newDetectedMarkers
				gameController.updateNewDetectedMarkers(markers.first);

				//place Buildings for long lasting markers
				gameController.placeBuildingsOnLongLastingMarkers(markers.first, markers.second);
				
				//update buildings
				for (int i = 0; i < go_buildings.size(); i++) {
					std::shared_ptr<GOBuilding> currentBuilding = go_buildings[i];
					if (currentBuilding->getBuildingType() != gameController.allBuildings[i].buildingType) {
						cout << "Updated Building" << endl;
						//update mesh
						switch (gameController.allBuildings[i].buildingType)
						{
						case Settlement:
							currentBuilding->setMesh(defaultSettlement->getMesh());
							break;
						case Road:
							currentBuilding->setMesh(defaultRoad->getMesh());
							break;
						case City:
							currentBuilding->setMesh(defaultCity->getMesh());
							break;
						}

						currentBuilding->setBuildingType(gameController.allBuildings[i].buildingType);
					}
				}

				//add new Buildings
				if (gameController.allBuildings.size() > go_buildings.size()) {
					for (int i = go_buildings.size(); i < gameController.allBuildings.size(); i++) {
						std::cout << "Added building" << std::endl;
						std::shared_ptr<GOBuilding> newBuilding;
						switch (gameController.allBuildings[i].buildingType)
						{
						case Settlement:
							newBuilding = std::make_shared<GOBuilding>(*defaultSettlement);
							break;
						case Road:
							newBuilding = std::make_shared<GOBuilding>(*defaultRoad);
							break;
						case City:
							newBuilding = std::make_shared<GOBuilding>(*defaultCity);
							break;
						}

						newBuilding->setModelMatrix(gameController.allBuildings[i].relativeToWorldReference);
						newBuilding->setColor(p.color);
						newBuilding->setHidden(false);
						newBuilding->setBuildingType(gameController.allBuildings[i].buildingType);
						go_buildings.push_back(newBuilding);
						gameObjects.push_back(newBuilding);
					}
				}
			}

			//draw background image
			int winWidth, winHeight;
			glfwGetFramebufferSize(window, &winWidth, &winHeight);
			cv_content.drawBackgroundImage(winWidth, winHeight);

			//render objects
			float deltaTime = glfwGetTime() - time;
			for (size_t j = 0; j < gameObjects.size(); j++)
			{
				gameObjects[j]->onUpdate(deltaTime);
				gameObjects[j]->onRender();
			}

			//render ui
			renderUI(p.resources.data(), gameController.lastDiceRoll,p.points, p.playerID, p.color);

			glfwSwapBuffers(window);

			// UI (GUI: event handling)
			UI.keyboard_EventHandler(window);

			//update time
			time = glfwGetTime();
		}

		cv_content.terminateBackgroundRendering();
		gameObjects.clear();
		go_markers.clear();
		go_buildings.clear();
		ui_textures.clear();
		defaultCity.reset();
		defaultRoad.reset();
		defaultSettlement.reset();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
