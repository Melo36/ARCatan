#include "CV_Content.h"


GLfloat vertices_bg[] =
{
-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
};

unsigned int indices_bg[] = {
        0, 1, 2,
        2, 3, 0
};


CV_Content::CV_Content()
{
    // Get video stream
    initVideoStream();

     markerTracker = std::make_unique<MarkerTracker>(kMarkerSize);
}

CV_Content::~CV_Content()
{
}

std::pair<int, int> CV_Content::getFrameSize()

{
    // Get image to find out camera characteristics
    cv::Mat img;
    cap >> img;


    return std::pair<int, int>(img.cols,img.rows);
}

void CV_Content::drawBackgroundImage(int winWidth, int winHeight)
{


    if (!m_currentFrame.empty()) {
        glm::mat4 mvp = m_cameraController->getViewProj(m_orthoCam) * m_model;
        m_shader->bind();
        
        m_texture->setTexture(m_currentFrame);
        m_texture->bind();
        m_shader->setUniformMat4f("u_MVP", mvp);
        m_shader->setUniform1i("cam_texture", 0);

        m_renderer->draw(*m_vao, *m_indexBuffer, *m_shader);
    }
}

std::pair<vector<int>, vector<glm::mat4>> CV_Content::getMarkers()
{
    Mat img;
    // Capture here 
    cap >> img;

    // Make sure that we got a frame -> otherwise crash
    if (img.empty()) {
        std::cout << "Could not query frame. Trying to reinitialize." << std::endl;
        initVideoStream();
        // Wait for one sec.
        cv::waitKey(1000);
    }
    else {
        // Track a marker and get the pose of the marker
        m_currentFrame = img.clone();

        vector <int> MarkerID;
        vector<Mat> MarkerPmat;

        markerTracker->findAllMarkers_pnp(img, MarkerID, MarkerPmat);

        vector<glm::mat4> convertedMat;
        for (int m = 0; m < MarkerID.size(); m++)
        {
            Mat Pmat = MarkerPmat[m];
            glm::mat4 converted = setUpCoordSys(Pmat);
            
            //let object sit atop marker instead of within
            //converted = glm::translate(converted, glm::vec3(0.0,0.0, kMarkerSize / 2.0));
            //scale from -1 to 1 coordinates to marer size
            converted = glm::rotate(converted, glm::radians(90.0f), glm::vec3(1.0,0.0, 0.0));
            //converted = glm::scale(converted, glm::vec3(kMarkerSize / 48.0));
            convertedMat.push_back(converted);
        }

        return std::pair<vector<int>, vector<glm::mat4>>(MarkerID, convertedMat);
    }


	return std::pair<vector<int>, vector<glm::mat4>>();
}

glm::mat4 CV_Content::setUpCoordSys(cv::Mat ocv_Pmat)
{
    // flip y-axis and z-axis
    // - to rotate CV-coordinate system (right-handed)
    // - to align with GL-coordinate system (right-handed)
    //   (see book "Demystified AR")
    //
    // also: transpose matrix to account for colum-major order of oGL arrays

    glm::mat4 glm_mat = { ocv_Pmat.at<double>(0,0), ocv_Pmat.at<double>(0,1), ocv_Pmat.at<double>(0,2), ocv_Pmat.at<double>(0,3),
                         -ocv_Pmat.at<double>(1,0), -ocv_Pmat.at<double>(1,1), -ocv_Pmat.at<double>(1,2), -ocv_Pmat.at<double>(1,3),
                         -ocv_Pmat.at<double>(2,0), -ocv_Pmat.at<double>(2,1), -ocv_Pmat.at<double>(2,2), -ocv_Pmat.at<double>(2,3),
                          0.0f, 0.0f, 0.0f, 1.0f };

    return glm::transpose(glm_mat);
}

void CV_Content::initVideoStream()
{
    if (cap.isOpened())
        cap.release();

    cap.open(0, cv::CAP_DSHOW);
    //cap.open("MarkerMovie.mp4");
    if (cap.isOpened() == false) {
        std::cout << "No webcam found, using a video file" << std::endl;
        cap.open("MarkerMovie.mp4");
        if (cap.isOpened() == false) {
            std::cout << "No video file found. Exiting." << std::endl;
            exit(0);
        }
    }
}

void CV_Content::initBackgroundRendering(GLFWwindow* window, CameraController* cameraController)
{
    m_vao = std::make_unique<VertexArray>();
    m_vertexBuffer = std::make_unique<VertexBuffer>(vertices_bg, sizeof(vertices_bg));

    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    m_vao->addBuffer(*m_vertexBuffer, layout);

    m_indexBuffer = std::make_unique<IndexBuffer>(indices_bg, (sizeof(indices_bg) / sizeof(unsigned int)));

    //load shader
     m_shader = std::make_unique<Shader>("res/shaders/ar_background.shader");

    //load texture
    Mat frame;
    cap >> frame;
    m_texture = std::make_unique<Texture>(frame);

    int winWidth, winHeight;
    glfwGetFramebufferSize(window, &winWidth, &winHeight);

    /*
    int max_d = cv::max(frame.size().width, frame.size().height);
    float fx = max_d;
    float fy = max_d;
    float cx = (float)frame.size().width / 2.0f;
    float cy = (float)frame.size().height / 2.0f;

    float depth = 499;
    m_model = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, -depth, 0, 0, 0, 1 };
    m_model = glm::transpose(m_model);
    m_model = glm::scale(m_model, glm::vec3(depth * 2 * cx / fx, depth * 2 * cy / fy, 0));
    */
    float orthoFar = 10000.0f;
    m_model = glm::translate(glm::scale(glm::mat4(1.0),glm::vec3(winWidth,winHeight,1.0)),glm::vec3(0.5,0.5,-orthoFar));
    

    m_cameraController = cameraController;
    m_orthoCam = cameraController->addOrthoCamera(cameraController->getView(), 0.0f, winWidth, 0.0f, winHeight, 0.1f, orthoFar);
}

void CV_Content::terminateBackgroundRendering()
{
    //clean up pointers
    m_vao.reset();
    m_vertexBuffer.reset();
    m_indexBuffer.reset();
    m_shader.reset();
    m_texture.reset();
    delete m_renderer;

}
