#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
//Includes opengl
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "opencv2/opencv.hpp"
#include "Shaders/ShaderProgram.h"
#include "Buffer/DeviceBuffer.h"
#include "Buffer/VertexBuffer.h"
#include "Logger.h"
using namespace cv;

const char* v_pthr =
"#version 330\n"
"in vec3 vPoints_M;"
"void main() {"
"	gl_Position = vec4(vPoints_M, 1.0);"
"}";

const char* f_pthr =
"#version 330\n"
"out vec4 vColor;"
"void main(){"
"	vColor = vec4(0.5, 0.0, 0.5, 1.0);"
"}";

void glfw_error_calllback(int error, const char* description)
{
	fprintf(stderr, "glfw error (%i): %s\n", error, description);
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

	glfwSetErrorCallback(glfw_error_calllback);
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	glfwWindowHint(GLFW_SAMPLES, 4);

    glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	diagnostics();

	float points[] = {
		0.f, 0.5f, 0.f,
		0.5f, -0.5f, 0.f,
		-0.5f, -0.5f, 0.f 
	};

	auto vbo = new DeviceBuffer();
	auto vao = new VertexBuffer();
	vbo->BindData(9 * sizeof(float), points);
	vao->Build();

	auto vs = new Shader(v_pthr, ShaderType::Vertex);
	auto fs = new Shader(f_pthr, ShaderType::Fragment);
	auto prog = new ShaderProgram();
	prog->AttachShader(vs);
	prog->AttachShader(fs);
	prog->Link();

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prog->Activate();
		vao->Draw();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
/*
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
