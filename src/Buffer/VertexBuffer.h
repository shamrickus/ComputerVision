#ifndef _VERTEX_BUFFER_H_
#define _VERTEX_BUFFER_H_

#include "GL/glew.h"
#include "GLFW/glfw3.h"

enum VertexDrawType
{
	Tri, Line, LineLoop
};

class VertexBuffer
{
public:
	VertexBuffer()
	{
		glGenVertexArrays(1, &buffer_);
		d = 1250;
		draw_ = Tri;
	}

	VertexBuffer(VertexDrawType pType) : VertexBuffer() 	{
		draw_ = pType;
	}

	bool Validate()
	{
		return glIsVertexArray(buffer_);
	}

	virtual void Bind()
	{
		glBindVertexArray(buffer_);
	}

	virtual void Draw(int pCount)
	{
		glBindVertexArray(buffer_);
		glDrawArrays(GetDrawType(), 0, pCount);
	}

	virtual void BindAttrib(DeviceBuffer* device, int index, int size)
	{
		glEnableVertexAttribArray(index);
		device->Bind();
		glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, NULL);

	}

	void SetType(VertexDrawType pType)
	{
		draw_ = pType;
	}

	unsigned char GetDrawType()
	{
		switch(draw_)
		{
		case Tri:
			return GL_TRIANGLES;
		case Line:
			return GL_LINES;
		case LineLoop:
			return GL_LINE_LOOP;
		}
	}

protected:
	GLuint buffer_;
	int d;
	VertexDrawType draw_;
};

#endif