
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
private:
	GLuint buffer_;
};
