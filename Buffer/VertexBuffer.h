
#include "GL/glew.h"
#include "GLFW/glfw3.h"

class VertexBuffer
{
public:
	VertexBuffer()
	{
		glGenVertexArrays(1, &buffer_);
	}

	void Build()
	{
		glBindVertexArray(buffer_);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	void Draw()
	{
		glBindVertexArray(buffer_);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	GLuint buffer_;
};