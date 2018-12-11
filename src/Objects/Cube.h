#ifndef _CUBE_H_
#define _CUBE_H_

#include "../Shaders/ShaderProgram.h"
#include "ode/ode.h"
#include "Object.h"
#include "PhysObject.h"

class Cube : public PhysObject
{
public:
	Cube(glm::vec3 pBase) : PhysObject(pBase)
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
		//auto ptns = ReadInOBJ("assets/cube.obj");
		//points_ = new float[ptns.size() * 3];
		//for(int i = 0; i < ptns.size(); ++i)
		//{
		//	points_[i * 3 + 0] = ptns[i].x;
		//	points_[i * 3 + 1] = ptns[i].y;
		//	points_[i * 3 + 2] = ptns[i].z;
		//}
		vbo_ = new DeviceBuffer();
		vao_ = new VertexBuffer();
		vbo_->BindData(12 * 3 * 3 * sizeof(float), points_);

		shader_ = new ShaderProgram();
		auto vs = new Shader(v_passthru, ShaderType::Vertex);
		auto fs = new Shader(f_passthru, ShaderType::Fragment);
		shader_->AttachShader(vs);
		shader_->AttachShader(fs);
		shader_->Link();

		size_ = glm::vec3(1);
		density_ = .1;
		color_ = glm::vec3(rand()%100/100.f, rand()%100/100.f, rand()%100/100.f);
	}

	void SetColor(glm::vec3 pColor) { color_ = pColor; }


	glm::mat4 Rotate(glm::mat4 mat, float pAngle, glm::vec3 pAxis)
	{
		float co = cos(pAngle);
		float si = sin(pAngle);
		float x2 = pAxis.x * pAxis.x;
		float y2 = pAxis.y * pAxis.y;
		float z2 = pAxis.z * pAxis.z;
		float x = pAxis.x;
		float y = pAxis.y;
		float z = pAxis.z;

		mat[0][0] = x2 + (y2 + z2) * co;
		mat[1][0] = x * y * (1 - co) - z * si;
		mat[2][0] = x * z * (1 - co) + y * si;
		mat[3][0] = 0.0f;

		mat[0][1] = x * y * (1 - co) + z * si;
		mat[1][1] = y2 + (x2 + z2) * co;
		mat[2][1] = y * z * (1 - co) - x * si;
		mat[3][1] = 0.0f;

		mat[0][2] = x * z * (1 - co) - y * si;
		mat[1][2] = y * z * (1 - co) + x * si;
		mat[2][2] = z2 + (x2 + y2) * co;
		mat[3][2] = 0.0f;

		mat[0][3] = 0.0f;
		mat[1][3] = 0.0f;
		mat[2][3] = 0.0f;
		mat[3][3] = 1.0f;

		return mat;
	}

	float* GetPoints()
	{
		return  points_;
	}

	glm::mat4 GetScale() override
	{
		return glm::scale(glm::identity<glm::mat4>(), glm::vec3(.1f));
	}

	void Draw(glm::mat4 pMVP) override
	{
		vao_->Bind();
		shader_->Activate();
		auto mvpHandle = shader_->GetHandle("MVP");
		auto mvp = pMVP * GetScreenTransform();
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &mvp[0][0]);

		auto colorHandle = shader_->GetHandle("nColor");
		glUniform3fv(colorHandle, 1,&color_[0]);

		vao_->BindAttrib(vbo_, 0, 3);
		vao_->Draw(12 * 3);

		auto outline = glm::vec3(0);
		glUniform3fv(colorHandle, 1,&outline[0]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		vao_->Bind();
		vao_->Draw(12 * 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glErrorCheck(__LINE__, __FILE__);
	}

	
	void Draw(glm::mat4 pMVP, glm::vec3 pColor) 
	{
		color_ = pColor;
		Draw(pMVP);
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
