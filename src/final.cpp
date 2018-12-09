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
#include "Timer.h"
#include "Perf.h"
#include "Handy/BackgroundRemover.h"
#include "Handy/SkinDetector.h"
#include "Handy/FingerCount.h"
#include "Handy/FaceDetector.h"
using namespace cv;


int main(int argc, char** argv)
{
	BackgroundRemover backgroundRemover;
	SkinDetector skinDetector;
	//FaceDetector faceDetector;
	FingerCount fingerCount;

	if (!CVLogger::Start())
		return -1;
	char out[1000];
	unsigned int seed = 100;
	sprintf(out, "Using seed %u\n", seed);
	CVLogger::Log(out);
	srand(seed);

	Perf::Init();
	GLFWwindow* window;
	Timer* timer = new Timer();

	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
		return -1;

	float height = 1080;
	float width = 1920;
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
	sprintf(out, "\nGL/GLFW/GLEW Init Took %.3fms\n", timer->Stop());
	CVLogger::Log(out);
	timer->Start();

	auto aruco = new Aruco("assets/test3");
	//auto aruco = new Aruco(0);
	sprintf(out, "CV Init %.3fms\n", timer->Stop());
	CVLogger::Log(out);
	timer->Start();

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
	sprintf(out, "World init took %.3fms\n", timer->Stop());
	CVLogger::Log(out);


	int frameNum = 1;
	int frameCount = 1;
	auto mem = Perf::GetMemoryInfo();
	sprintf(out, "Process Memory: %.2fMB, Peak: %.2fMB\n", (float)mem.usedBy/(1024*1024), (float)mem.usedByPeak/(1024*1024));
	CVLogger::Log(out);
	int req = 60;
	auto t2 = new Timer();

	std::vector< std::map<std:: string, double > > frameStats_;
	do
	{
		std::map<std::string, double> times;
		timer->Start();
		t2->Start();
		auto frame = aruco->ProcessFrame();
		auto image = frame->Image();
		skinDetector.drawSkinColorSampler(image);
		auto fg = backgroundRemover.getForeground(image);
		auto mask = skinDetector.getSkinMask(fg);
		auto fc = fingerCount.findFingersCount(mask, image);
		if (frameCount < req/2)
			backgroundRemover.calibrate(image);

		if (frameCount == req)
			skinDetector.calibrate(image);

		if (frameCount > req) {
			cv::imshow("out", image);
			cv::imshow("fg", fg);
			cv::imshow("mask", mask);
			cv::imshow("hand", fc.first);
		}
		times["Handy"] = t2->Stop();
		frame->ToTexture();

		screen->SetTexture(frame->ToTexture());
		times["CV"] = timer->Restart(); 

		timer->Start();
		drawStatistics(window, frameCount++);
		
		auto mvp = proj; //* cam->View();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		auto t = (*frame)[frameNum]->GLTransform(scale);
		screen->Draw(mvp, true);
		glClear(GL_DEPTH_BUFFER_BIT);
		auto rot = glm::rotate(proj, 3.14f / 4, glm::vec3(0, 1, 0))*t;
		auto rot2 = glm::rotate(proj, -3.14f / 4, glm::vec3(0, 0, 1))*t;
		rot2 = glm::rotate(rot2, -3.14f / 4, glm::vec3(1, 0, 0))*t;
		grid->SetPosition(glm::vec3(1, 0, 0));
		//grid->Draw(glm::scale(proj, glm::vec3(0.2, 0.2, 0.2)));
		//grid->Draw(glm::scale(rot2, glm::vec3(0.2, 0.2, 0.2)));

		for (int i = 0; i < cubes.size(); ++i)
		{
			cubes[i]->SetPosition(glm::vec3(0));
			cubes[i]->Draw(rot, Yellow());
			cubes[i]->SetPosition(glm::vec3(0.065,0,0.235));
			cubes[i]->Draw(rot, Green());
		}
		axis->Draw(mvp);

		glfwSwapBuffers(window);

		glfwPollEvents();
		times["CPUDraw"] = (timer->Restart());

		cam->Update(window);
		times["CamUpdate"] = (timer->Restart());

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break;

		world->Update(1.f / 60);
		times["Physics"] = timer->Restart();

		glErrorCheck(__LINE__, __FILE__);
		frameStats_.push_back(times);

		if(frameCount % 100 == 0)
		{
			char out[1024];
			sprintf(out, "Frame %i:\nHandy Time: %.3f, CV Time: %.3f, CPUDraw Time: %.3f, CamUpdate Time: %.3f, Phsyics Time: %.3f\n",
				frameCount, times["Handy"], times["CV"], times["CPUDraw"], times["CamUpdate"], times["Physics"]);
			CVLogger::Log(out);
			mem = Perf::GetMemoryInfo();
			sprintf(out, "Mem used: %.2f, Peak: %.2f\n", (float)mem.usedBy / (1024 * 1024), (float)mem.usedByPeak / (1024 * 1024));
		CVLogger::Log(out);
		}
	} while (!glfwWindowShouldClose(window));

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
