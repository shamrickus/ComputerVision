#ifndef _DEVICE_BUFFER_H_
#define _DEVICE_BUFFER_H_

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class DeviceBuffer
{
public:
	DeviceBuffer()
	{
		glGenBuffers(1, &buffer_);
	}

	void Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	}

	void BindData(int pSize, float* pData)
	{
		Bind();
		glBufferData(GL_ARRAY_BUFFER, pSize, pData, GL_STATIC_DRAW);
	}

	bool Validate()
	{
		return glIsBuffer(buffer_);
	}
private:
	GLuint buffer_;
};

#endif
