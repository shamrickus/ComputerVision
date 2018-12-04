#ifndef _CUBE_H_
#define _CUBE_H_

#include "../Shaders/ShaderProgram.h"
#include "ode/ode.h"
#include "Object.h"

class Cube : public Object
{
public:
	Cube(glm::vec3 pBase) : Object(pBase)
	{
		points_ = new float[12*9]{
			-1.0f,-1.0f,-1.0f, // triangle 1 : begin
			-1.0f,-1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f, // triangle 1 : end
			1.0f, 1.0f,-1.0f, // triangle 2 : begin
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f, // triangle 2 : end
			1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			-1.0f,-1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f,-1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f,-1.0f,
			1.0f,-1.0f,-1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f,-1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f,-1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f,-1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f,-1.0f, 1.0f
		};

		vbo_ = new DeviceBuffer();
		vao_ = new VertexBuffer();
		vbo_->BindData(12 * 3 * 3 * sizeof(float), points_);
		vao_->Build();

		shader_ = new ShaderProgram();
		auto vs = new Shader(v_passthru, ShaderType::Vertex);
		auto fs = new Shader(f_passthru, ShaderType::Fragment);
		shader_->AttachShader(vs);
		shader_->AttachShader(fs);

		shader_->Link();
		size_ = glm::vec3(2);
		density_ = 5;
		color_ = glm::vec3(1);
	}

	void SetPosition(glm::vec3 pPos)
	{
		base_ = pPos;
	}

	void Draw(glm::mat4 pMVP)
	{

		vbo_->Bind();
		shader_->Activate();
		auto mvpHandle = shader_->GetHandle("MVP");
		auto mvp = pMVP * GetTranslation();
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &mvp[0][0]);

		auto colorHandle = shader_->GetHandle("nColor");
		glUniform3fv(colorHandle, 1,&color_[0]);
		vao_->Draw(12 * 3);

		auto outline = glm::vec3(0);
		glUniform3fv(colorHandle, 1,&outline[0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		vao_->Draw(12 * 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glErrorCheck(__LINE__, __FILE__);
	}

	~Cube()
	{
		delete points_;
		delete shader_;
		delete vao_;
		delete vbo_;
	}
private:
	DeviceBuffer * vbo_;
	VertexBuffer* vao_;
	float *points_;
	ShaderProgram* shader_;
	glm::vec3 color_;
};

#endif
