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
#include "opencv2/aruco/dictionary.hpp"
#include "opencv2/aruco.hpp"
#include "Objects/Aruco.h"
#include "Objects/Screen.h"
#include "Colors.h"
#include "Axis.h"
using namespace cv;


int main(int argc, char** argv)
{
		srand(100);
	if (!CVLogger::Start())
		return -1;


	auto vid = VideoCapture("assets/pixel/test.mp4");
	Mat image;
	vid >> image;
	 //= imread("assets/example1.png", IMREAD_COLOR); 
	if (image.empty())
	{
		CVLogger::Log("Could not open or find the image");
		return -1;
	}

	auto aruco = new Aruco();
	auto tag = aruco->ProcessFrame(image, 1);
//ShowImageInWindow(image);

    GLFWwindow* window;

	glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
        return -1;

	float height = 1080;
	float width = 1920;
	/*
	float height = 800;
	float width = tag->HWRatio() * 800;
    */
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

	bool dim2 = false;

	if(!dim2)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	glClearColor(0.1, 0.1, 0.1, 1);

	diagnostics();
	auto proj = glm::perspective(45.f, (float)width/float(height), 0.1f, 1000.f);

	auto world = new World(glm::vec3(0, -2,  0));
	auto cam = new Camera();
	Grid* grid = new Grid(glm::vec3(0.f, 1.f, 0.f));
	std::vector<Cube*> cubes;
	world->AddBody(grid, false, 3ul, ~1ul, true);
	for(int i = 0; i < 1; ++i )
	{
		auto cub = new Cube(glm::vec3(0));
		cub->SetSize(glm::vec3(1));
		world->AddBody(cub, false, 2ul, ~1ul, false);
		cubes.push_back(cub);
	}
	auto axis = new Axis(glm::vec3(0));

	glErrorCheck(__LINE__, __FILE__);
	Screen* screen;
	if(dim2)
		screen = new Screen(glm::vec3(0.f), glm::vec3(width, height, 0));
	else
		screen = new Screen(glm::vec3(1.f, 1.f, 0.f), glm::vec3(tag->HWRatio(), 1, 1)*3.f);
	screen->SetTexture(tag->ToTexture());

    while (!glfwWindowShouldClose(window))
    {
		auto timeStep = drawStatistics(window);
		auto ortho = glm::ortho(0.f, (float)width, 0.f, (float)height);
		auto mvp = proj * cam->View();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/* screen->Draw(ortho, true);
		glEnable(GL_DEPTH_TEST);
		auto t = (*tag)[1]->GLTransform();
		//cube2->Draw(glm::ortho(0.f, width, 0.f, height));
		glDisable(GL_DEPTH_TEST); */
		//grid->Draw(mvp);
		auto t = (*tag)[1]->GLTransform();
		for(int i = 0; i < cubes.size(); ++i)
		{
			cubes[i]->Draw(mvp * t, Blue());
			cubes[i]->Draw(mvp * screen->GetTranslation() * t, Red());
		}
		screen->Draw(mvp, false);
		axis->Draw(mvp);
		//mvp = glm::rotate(mvp, 3.14f / 2, glm::vec3(1, 0, 0));
		//screen->Draw(glm::translate(mvp, glm::vec3(-15, -15,-1.1)), false);
		/*
		*/
		/*
		auto x = glm::rotate(glm::identity<glm::mat4>(), 3.124f / 2, glm::vec3(1, 1, 1));
		cube2->Draw(mvp*screen->GetScreenTransform() * t * x);
		cube2->Draw(mvp * t);
		*/

        glfwSwapBuffers(window);

        glfwPollEvents();
		cam->Update(window);

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || 
			glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break;

		world->Update(1.f/90);
		glErrorCheck(__LINE__, __FILE__);
    }

	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
