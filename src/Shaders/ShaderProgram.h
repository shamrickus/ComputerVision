#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include "Shader.h"

class ShaderProgram
{
public:
	ShaderProgram()
	{
		program_ = glCreateProgram();
	}
	
	void AttachShader(Shader* pShader)
	{
		glAttachShader(program_, pShader->Handle());
	}

	bool IsLinked()
	{
		GLint linked;
		glGetProgramiv(program_, GL_LINK_STATUS, &linked);
		return linked;
	}

	void Activate()
	{
		glUseProgram(program_);
	}

	void Deactivate()
	{
		glUseProgram(0);
	}

	void Link()
	{
		glLinkProgram(program_);

		if (!IsLinked() || !Validate(program_))
			ProgramLog(program_);
	}

	GLuint GetHandle(const char* pStr)
	{
		return glGetUniformLocation(program_, pStr);
	}

	static bool IsProgram(GLuint pProgram)
	{
		return glIsProgram(pProgram);
	}
private:
	GLuint program_;

	static void ProgramLog(GLuint pProgram)
	{
		GLint length;
		glGetProgramiv(pProgram, GL_INFO_LOG_LENGTH, &length);
		auto log = (GLchar*)malloc(length);
		glGetProgramInfoLog(pProgram, length, &length, log);
		fprintf(stderr, "program error %s\n", log);
		free(log);
	}

	static bool Validate(GLuint pProgram)
	{
		glValidateProgram(pProgram);
		GLint status;
		glGetProgramiv(pProgram, GL_VALIDATE_STATUS, &status);

		return status;
	}
};

#endif
