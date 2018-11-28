#ifndef _VERTEX_BUFFER_H_
#define _VERTEX_BUFFER_H_

#include "GL/glew.h"
#include "GLFW/glfw3.h"

enum VertexDrawType
{
	Tri, Line
};

class VertexBuffer
{
public:
	VertexBuffer() : draw_(Tri)
	{
		glGenVertexArrays(1, &buffer_);
	}

	VertexBuffer(VertexDrawType pType) : VertexBuffer() 	{
		draw_ = pType;
	}

	bool Validate()
	{
		return glIsVertexArray(buffer_);
	}

	void Build()
	{
		glBindVertexArray(buffer_);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	}

	void Draw(int pCount)
	{
		glBindVertexArray(buffer_);
		auto dt = GetDrawType();
		glDrawArrays(GetDrawType(), 0, pCount);
	}

	unsigned char GetDrawType()
	{
		switch(draw_)
		{
		case Tri:
			return GL_TRIANGLES;
		case Line:
			return GL_LINES;
		}
	}

private:
	GLuint buffer_;
	VertexDrawType draw_;
};

#endif