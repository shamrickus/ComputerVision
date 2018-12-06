#ifndef _SHADER_H_
#define _SHADER_H_

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <cstdlib>
#include <cstdio>

const char* v_passthru = 
"#version 330\n" 
"in vec3 vPoints_M;\n" 
"uniform mat4 MVP;\n" 
"void main() { \n" 
	"gl_Position = MVP * vec4(vPoints_M, 1.0);\n" 
"}\n" ;
const char* v_2D_xz_passthru = 
"#version 330\n" 
"in vec3 vPoints_M;\n" 
"uniform mat4 MVP;\n" 
"void main() { \n" 
	"gl_Position = MVP * vec4(vPoints_M.xz, 0.0, 1.0);\n" 
"}\n" ;
const char* f_passthru = 
"#version 330\n" 
"out vec4 color;\n"
"uniform vec3 nColor;\n"
"void main() { \n"
	"color = vec4(nColor, 1.0);\n"
"}\n" ;

const char* v_texture = 
"#version 330\n" 
"in vec3 vPoints_M;\n" 
"in vec2 vUV;\n"
"uniform mat4 MVP;\n" 
"out vec2 uv;\n"
"void main() { \n" 
	"gl_Position = MVP * vec4(vPoints_M.xz, 0.0, 1.0);\n" 
	"uv = vUV;\n"
"}\n" ;
const char* f_texture =
"#version 330\n"
"in vec2 uv;\n"
"uniform sampler2D texture;\n"
"out vec4 color;\n"
"void main() {\n"
	"color = texture2D(texture, uv);\n"
"}\n";

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
		default:
		case Vertex:
			return glCreateShader(GL_VERTEX_SHADER);
		case Fragment:
			return glCreateShader(GL_FRAGMENT_SHADER);
		}
	}
};

#endif
