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
#include "Console.h"
using namespace cv;
float rotX, rotY, rotZ;

int rotation(std::vector<std::string>& pTokens, void*)
{
	if (pTokens[1] == "x")
		rotX = stof(pTokens[2]);
	if (pTokens[1] == "y")
		rotY = stof(pTokens[2]);
	if (pTokens[1] == "z")
		rotZ = stof(pTokens[2]);

	return 1;
}

int main(int argc, char** argv)
{
	BackgroundRemover backgroundRemover;
	SkinDetector skinDetector;
	//FaceDetector faceDetector;
	FingerCount fingerCount;

	if (!CVLogger::Start())
		return -1;
	char out[1000];
	unsigned int seed = time(nullptr);
	sprintf(out, "Using seed %u\n", seed);
	CVLogger::Log(out);
	srand(seed);

	Perf::Init();
	GLFWwindow* window;
	Timer* timer = new Timer();
	Console::Register("a", &rotation);

	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
		return -1;

	float height = 1080/2;
	float width = 1920/2;
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!window)
	{
		CVLogger::Log("Unable to create window!");
		glfwTerminate();
		return -1;
	}

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
	sprintf(out, "CV Init %.3fms\n", timer->Stop());
	CVLogger::Log(out);
	timer->Start();

	float n = 0.1f;
	float f = 1000.f;
	auto proj = BuildProjection(50.f, width / height, n, f);


	auto world = new World(glm::vec3(-0.4, -0.6, -0.7)*2.f);
	auto cam = new Camera();
	auto grid = new Grid(glm::vec3(0.f, 1.0f, 0.f));
	std::vector<Cube*> cubes;
	world->AddBody(grid, false, 3ul, ~1ul, true);
	grid->SetPosition(glm::vec3(0, 1, -0.4));
	for (int i = 0; i < 8; ++i)
	{
		auto cub = new Cube(glm::vec3(0, 6.5+(rand() % 5 / 15.f),0.75+(rand()%5 / 3.f)));
		cub->SetSize(glm::vec3(.2f));
		world->AddBody(cub, false, 2ul, ~1ul, false);
		cub->SetRotation(glm::vec3(rand() % 3/1.f, rand() % 3 / 1.f, rand() % 3/1.f));
		cubes.push_back(cub);
	}

	Screen* screen;
	float scale = 1.f;

	screen = new Screen(glm::vec3(0.f), glm::vec3(width, height, 0));
	sprintf(out, "World init took %.3fms\n", timer->Stop());
	CVLogger::Log(out);


	int frameCount = 1;
	auto mem = Perf::GetMemoryInfo();
	sprintf(out, "Process Memory: %.2fMB, Peak: %.2fMB\n", (float)mem.usedBy/(1024*1024), (float)mem.usedByPeak/(1024*1024));
	CVLogger::Log(out);
	int req = 60;
	sprintf(out, "Using %i frames of calibration for hand detection\n", req);
	CVLogger::Log(out);

	ArucoFrame* frame;
	rotX = 5.9; rotY = 1.8; rotZ = -2.19;
	double last = 15;
	bool wait = true;
	std::vector< std::map<std:: string, double > > frameStats_;
	do
	{
		/*if(wait)
		{
			glfwSwapBuffers(window);

			glfwPollEvents();
			if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
				wait = false;
			else
				continue;
		}*/

		std::map<std::string, double> times;
		timer->Start();
		auto frameNew = aruco->ProcessFrame();
		if (frameNew != nullptr)
			frame = frameNew;
		auto image = frame->Image();
		times["CV"] = timer->Restart(); 

		skinDetector.drawSkinColorSampler(image);
		auto fg = backgroundRemover.getForeground(image);
		auto mask = skinDetector.getSkinMask(fg);
		auto fc = fingerCount.findFingersCount(mask, image);
		if (frameCount < req/2)
			backgroundRemover.calibrate(image);

		if (frameCount == req)
			skinDetector.calibrate(image);

		if (frameCount > req) {
			if(fc.second.size() > 3 && last++>15)
			{
				auto cub = new Cube(glm::vec3(0, 5.5+(rand() % 5 / 10.f),0.75+(rand()%5 / 3.f)));
				cub->SetSize(glm::vec3(.2f));
				cub->SetColor(glm::vec3(1));
				world->AddBody(cub, false, 2ul, ~1ul, false);
				cub->SetRotation(glm::vec3(rand() % 3/1.f, rand() % 3 / 1.f, rand() % 3/1.f));
				cubes.push_back(cub);
				last = 0;
			}
			//cv::imshow("out", image);
			//cv::imshow("fg", fg);
			//cv::imshow("mask", mask);
			//cv::imshow("hand", fc.first);
		}
		screen->SetTexture(frame->ToTexture());
		times["Handy"] = timer->Restart();


		timer->Start();
		drawStatistics(window, frameCount++);
		
		auto mvp = proj; //* cam->View();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		screen->Draw(mvp, true);
		glClear(GL_DEPTH_BUFFER_BIT);
		auto proj2 = glm::rotate(proj, -3.14f / 2, glm::vec3(rotX,rotY,rotZ));
		dMatrix3 r;
		auto d = frame->GetGround();
		r[0] = d[0][2]; r[1] = d[0][1]; r[2] = d[0][0]; r[3] = 0;
		r[4] = d[1][2]; r[5] = d[1][1]; r[6] = d[1][0]; r[7] = 0;
		r[8] =  d[2][2]; r[9] =  d[2][1]; r[10] = d[2][0]; r[11] = 0;

		grid->SetRotation(r);
		for (int i = 0; i < cubes.size(); ++i)
		{
			cubes[i]->Draw(proj2*glm::translate(glm::identity<glm::mat4>(), glm::vec3(0,-5,-1)));
		}
		//axis->Draw(proj);

		glfwSwapBuffers(window);

		glfwPollEvents();
		times["CPUDraw"] = (timer->Restart());

		cam->Update(window);
		times["CamUpdate"] = (timer->Restart());

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break;

		float num = 0.1;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			num *= -1;
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
			rotX += num;
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			rotY += num;
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
			rotZ += num;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			Console::Activate();

		world->Update(1.f / 60);
		times["Physics"] = timer->Restart();

		glErrorCheck(__LINE__, __FILE__);
		frameStats_.push_back(times);

		if(frameCount % 50 == 0)
		{
			char out[1024];
			sprintf(out, "Frame %i:\nHandy Time: %.3f, CV Time: %.3f, CPUDraw Time: %.3f, CamUpdate Time: %.3f, Phsyics Time: %.3f\n",
				frameCount, times["Handy"], times["CV"], times["CPUDraw"], times["CamUpdate"], times["Physics"]);
			CVLogger::Log(out);
			mem = Perf::GetMemoryInfo();
			sprintf(out, "Mem used: %.2f, Peak: %.2f\n", (float)mem.usedBy / (1024 * 1024), (float)mem.usedByPeak / (1024 * 1024));
			CVLogger::Log(out);
		}
		if(frameCount % 220 == 0)
		{
			Dump(frameStats_);
		}
	} while (!glfwWindowShouldClose(window));

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
