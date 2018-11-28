#include "reactphysics3d.h"

#ifdef _WIN32
#include "windows.h"
#endif

//Includes opengl
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "opencv2/opencv.hpp"
#include "Shaders/ShaderProgram.h"
#include "Buffer/DeviceBuffer.h"
#include "glm/glm.hpp"
#include "Buffer/VertexBuffer.h"
#include "Logger.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Objects/Cube.h"
#include "Objects/Camera.h"
#include "Objects/Grid.h"
#include "Helper.h"
using namespace cv;

void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "glfw error (%i): %s\n", error, description);
	Logger::Log(description);
}

void drawStatistics(GLFWwindow* window)
{
	static double lastMeasuredTime = glfwGetTime();
	static int framesSinceLastUpdate;
	static size_t frameCount;
	double currentTime = glfwGetTime();
	double elapsed = currentTime - lastMeasuredTime;
	if(elapsed > (double)1/15)
	{
		lastMeasuredTime = currentTime;
		char outputText[256];
		sprintf(outputText, "FPS: %.0f, FT: %.4fms, F: %llu",
			((float)framesSinceLastUpdate / elapsed), 
			elapsed / framesSinceLastUpdate, frameCount);
		glfwSetWindowTitle(window, outputText);
		framesSinceLastUpdate = 0;
	}
	framesSinceLastUpdate++;
	frameCount++;
}

void diagnostics()
{
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	char* out = new char[100];
	sprintf(out, "Renderer: %s\n", renderer);
	Logger::Log(out);
	sprintf(out, "OpenGL version supported %s\n", version);
	Logger::Log(out);
}

int main()
{
	if (!Logger::Start())
		return -1;
    GLFWwindow* window;

	glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        return -1;

	int height = 768;
	int width = 1280;
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
		Logger::Log("Unable to create window!");
        glfwTerminate();
        return -1;
    }

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	diagnostics();
	auto model = glm::mat4(1.f);
	auto proj = glm::perspective(45.f, (float)width/float(height), 0.1f, 1000.f);
	rp3d::DynamicsWorld world(rp3d::Vector3(0.f, -9.81, 0.f), rp3d::WorldSettings());
	world.setNbIterationsPositionSolver(10);
	world.setNbIterationsVelocitySolver(10);
	world.enableSleeping(true);
	world.setIsGratityEnabled(true);

	auto origin = glm::vec3(0.f, -1.f, 0.f);
	Grid* grid = new Grid(&origin);
	Cube* cube = new Cube(&world);
	auto cam = new Camera();
	auto mvp = proj * cam->View() * model;

	glErrorCheck(__LINE__, __FILE__);

    while (!glfwWindowShouldClose(window))
    {
		drawStatistics(window);
		mvp = proj * cam->View() * model;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid->Draw(mvp);
		cube->Draw(mvp);

        glfwSwapBuffers(window);

        glfwPollEvents();
		cam->Update(window);
		//world.update(1 / 30.f);

		glErrorCheck(__LINE__, __FILE__);
    }

	glfwDestroyWindow(window);
	delete grid, cube;
    glfwTerminate();
    return 0;
}
/*
#include "iostream"
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << " Usage: display_image ImageToLoadAndDisplay" << std::endl;
		return -1;
	}
	Mat image;
	image = imread(argv[1], IMREAD_COLOR); // Read the file
	if (image.empty()) // Check for invalid input
	{
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", image); // Show our image inside it.
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}
*/
