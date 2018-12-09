#ifdef _WIN32
#include "windows.h"
#endif

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
#include "Aruco/Aruco.h"
#include "Objects/Screen.h"
#include "Colors.h"
#include "Objects/Axis.h"
using namespace cv;


int main(int argc, char** argv)
{
	if (!CVLogger::Start())
		return -1;
	srand(100);

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.1, 0.1, 0.1, 1);
	glLineWidth(3.f);

	diagnostics();

	Mat image;

	auto vid = VideoCapture("assets/pixel/test2.mp4");
	vid >> image;
	//image = imread("assets/pixel/test.jpg", IMREAD_COLOR);
	if (image.empty())
	{
		CVLogger::Log("Could not open or find the image");
		return -1;
	}

	auto aruco = new Aruco();
	auto tag = aruco->ProcessFrame(image, 1);

	float n = 0.1f;
	float f = 1000.f;
	auto intrin = aruco->CameraIntrin();
	float a = (float)intrin.at<float>(0, 0);
	float b = (float)intrin.at<float>(1, 1);
	float cx = (float)intrin.at<float>(0, 2);
	float cy = (float)intrin.at<float>(1, 2);
	auto proj2 = glm::perspective(45.f, (float)width / float(height), 0.1f, 1000.f);
	auto proj1 = BuildProjectionLegacy(n, f, a, b, cx, cy);
	auto proj = BuildProjection(50.f, width / height, n, f);




	auto world = new World(glm::vec3(0, -2, 0));
	auto cam = new Camera();
	Grid* grid = new Grid(glm::vec3(0.f, 1.f, 0.f));
	std::vector<Cube*> cubes;
	world->AddBody(grid, false, 3ul, ~1ul, true);
	for (int i = 0; i < 1; ++i)
	{
		auto cub = new Cube(glm::vec3(0));
		cub->SetSize(glm::vec3(.005f));
		world->AddBody(cub, false, 2ul, ~1ul, false);
		cubes.push_back(cub);
	}
	auto axis = new Axis(glm::vec3(0));

	Screen* screen;
	float scale = 1.f;

	screen = new Screen(glm::vec3(0.f), glm::vec3(width, height, 0));
	//screen = new Screen(glm::vec3(0.f), glm::vec3(tag->HWRatio(), 1, 1) * scale);
	screen->SetTexture(tag->ToTexture());
	int frame = 1;

	while (!glfwWindowShouldClose(window))
	{
		auto timeStep = drawStatistics(window);
		auto ortho = glm::ortho(0.f, (float)width, 0.f, (float)height);
		auto view = glm::identity<glm::mat4>();
		view[0][0] = 0;
		view[1][1] = 0;
		view[2][2] = 0;
		view[3][3] = 1;
		auto mvp = proj * view; //* cam->View();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		auto t = (*tag)[frame]->GLTransform(scale);
		auto tg = (*tag)[frame];
		screen->Draw(mvp, true);
		glClear(GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < cubes.size(); ++i)
		{
			cubes[i]->SetPosition(glm::vec3(0));
			//cubes[i]->Draw(proj*t, Yellow());
			cubes[i]->SetPosition(glm::vec3(0.065,0,0.235));
			cubes[i]->Draw(glm::rotate(proj, 3.14f/4, glm::vec3(0,1,0))*t, Green());
			/*auto corner = tg->Corner();
			auto retT = glm::rotate(t, 3.14f/2, glm::vec3(0, 1, 0));
			corner[0] /= 1000;
			cubes[i]->SetPosition(glm::vec3(-corner[0].x/2, 0.f, -corner[0].y/2));
			//cubes[i]->Draw(t, Red());

			cubes[i]->SetPosition(glm::vec3(0));
			auto proj = aruco->Projection(0.1f, 100.f, 0, 0, 5.f, 5.f);
			auto mat4 = glm::make_mat4x4(proj);
			//cubes[i]->Draw(t, Yellow());

			cubes[i]->SetPosition(glm::vec3(0));
			auto rot = tg->RotationRaw();
			auto x = tg->Corner()[0].x;
			auto y = tg->Corner()[0].y;
			auto calc = glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, y, y));
			calc = glm::rotate(calc, 57.13f*(float)cv::norm(tg->RotationRaw()), glm::vec3((float)rot[0], -rot[1], -rot[2]));
			//cubes[i]->Draw(calc, Teal(), true);

			cubes[i]->SetPosition(glm::vec3(corner[0].x/2, 0.f, corner[0].y/2));
			//cubes[i]->Draw(glm::transpose(t), Green());*/
		}
		axis->Draw(mvp);

		glfwSwapBuffers(window);

		glfwPollEvents();
		cam->Update(window);

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break;

		world->Update(1.f / 60);
		glErrorCheck(__LINE__, __FILE__);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
