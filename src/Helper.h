#ifndef _HELPER_H_
#define _HELPER_H_
#include "Buffer/VertexBuffer.h"
#include "CVLogger.h"

void glErrorCheck(int pLine, const char* pFile)
{
	auto err = glGetError();
	if(err != GL_NO_ERROR)
	{
		auto errStr = gluErrorString(err);
		char out[1024];
		sprintf(out, "ERR: %s, %i %s\n", (char*)errStr, pLine, pFile);
		CVLogger::Log(out);
	}
}

float* ODEtoOGL(const dReal* p, const dReal * R)
{
	float* M = new float[15];
	M[0] = R[0]; M[1] = R[4]; M[2] = R[8];  M[3] = 0;
	M[4] = R[1]; M[5] = R[5]; M[6] = R[9];  M[7] = 0;
	M[8] = R[2]; M[9] = R[6]; M[10] = R[10]; M[11] = 0;
	M[12] = p[0]; M[13] = p[1]; M[14] = p[2];  M[15] = 1;
	return M;
}

void diagnostics()
{
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	char* out = new char[100];
	sprintf(out, "Renderer: %s\n", renderer);
	CVLogger::Log(out);
	sprintf(out, "OpenGL version supported %s\n", version);
	CVLogger::Log(out);
}

void glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "glfw error (%i): %s\n", error, description);
	CVLogger::Log(description);
}

double drawStatistics(GLFWwindow* window)
{
	static double lastMeasuredTime = glfwGetTime();
	static int framesSinceLastUpdate=0;
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
	frameCount++;

	return elapsed / (1 + framesSinceLastUpdate++);
}


#endif