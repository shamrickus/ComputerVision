#ifndef _HELPER_H_
#define _HELPER_H_
#include "Buffer/VertexBuffer.h"
#include "Logger.h"

void glErrorCheck(int pLine, const char* pFile)
{
	auto err = glGetError();
	if(err != GL_NO_ERROR)
	{
		auto errStr = gluErrorString(err);
		char out[1024];
		sprintf(out, "ERR: %s, %i %s\n", (char*)errStr, pLine, pFile);
		Logger::Log(out);
	}
}

#endif