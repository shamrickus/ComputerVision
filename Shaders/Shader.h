
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdlib>
#include <cstdio>

enum ShaderType
{
	Vertex, Fragment
};

class Shader
{
public:
	Shader(const char* pShaderText, ShaderType pType) : type_(pType)
	{
		shader_ = Shader::CreateShader(pType);
		glShaderSource(shader_, 1, &pShaderText, nullptr);
		glCompileShader(shader_);

		if (!IsCompiled())
			GetLog(shader_);
	}

	GLuint Handle()
	{
		return shader_;
	}

	bool IsCompiled()
	{
		GLint compiled;
		glGetShaderiv(shader_, GL_COMPILE_STATUS, &compiled);
		return compiled;
	}

	static bool IsShader(GLuint pShader)
	{
		return glIsShader(pShader);
	}
private:
	ShaderType type_;
	GLuint shader_;

	static void GetLog(GLuint pShader) {
		GLint length;
		glGetShaderiv(pShader, GL_INFO_LOG_LENGTH, &length);
		auto log = (GLchar*)malloc(length);
		glGetShaderInfoLog(pShader, length, &length, log);
		fprintf(stderr, "shader error %s\n", log);
		free(log);
	}
	static GLuint CreateShader(ShaderType pType)
	{
		switch(pType)
		{
		case Vertex:
			return glCreateShader(GL_VERTEX_SHADER);
		case Fragment:
			return glCreateShader(GL_FRAGMENT_SHADER);
		}
	}
};
