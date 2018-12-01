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
#include "CVLogger.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Objects/Cube.h"
#include "Objects/Camera.h"
#include "Objects/Grid.h"
#include "Helper.h"
#include "ode/ode.h"
#include "Objects/World.h"
using namespace cv;

int main()
{
	if (!CVLogger::Start())
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
		CVLogger::Log("Unable to create window!");
        glfwTerminate();
        return -1;
    }

	glEnable(GL_MULTISAMPLE);
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
	glDepthFunc(GL_LEQUAL);

	diagnostics();
	auto proj = glm::perspective(45.f, (float)width/float(height), 0.1f, 1000.f);

	auto world = new World(glm::vec3(0, -2,  0));
	auto cam = new Camera();
	Grid* grid = new Grid(glm::vec3(0.f, -.25f, 0.f));
	Cube* cube = new Cube(glm::vec3(0.f, 2.f, 0.f));
	Cube* cube2 = new Cube(glm::vec3(0.5f, 7.f, 0.f));
	world->AddBody(grid, false, 3ul, ~1ul, true);
	world->AddBody(cube, false, 2ul, ~1ul, false);
	world->AddBody(cube2, false, 2ul, ~1ul, false);

	glErrorCheck(__LINE__, __FILE__);

    while (!glfwWindowShouldClose(window))
    {
		auto timeStep = drawStatistics(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto mvp = proj * cam->View();
		grid->Draw(mvp);
		cube->Draw(mvp);
		cube2->Draw(mvp);

        glfwSwapBuffers(window);

        glfwPollEvents();
		cam->Update(window);

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break;

		world->Update(1.f/60);
		glErrorCheck(__LINE__, __FILE__);
    }

	glfwDestroyWindow(window);
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
