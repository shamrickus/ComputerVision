#ifndef _AXIS_H_
#define _AXIS_H_
#include "Objects/Object.h"
#include "Buffer/DeviceBuffer.h"
#include "Shaders/ShaderProgram.h"
#include <vector>
#include "Colors.h"

class Axis : public Object
{
	
public:
	Axis(glm::vec3 pBase) : Object(pBase)
	{
		program_ = new ShaderProgram();
		auto vs = new Shader(v_passthru, Vertex);
		auto fs = new Shader(f_passthru, Fragment);
		program_->AttachShader(vs);
		program_->AttachShader(fs);
		program_->Link();

		size_ = glm::vec3(1);

		float line[3 * 2];
		line[0] = 0;
		line[1] = 0;
		line[2] = 0;
		for(int i = 0; i < 3; ++i)
		{
			int x = i == 0 ? 1 : 0;
			int y = i == 1 ? 1 : 0;
			int z = i == 2 ? 1 : 0;

			line[3] = x;
			line[4] = y;
			line[5] = z;
			auto vbo = new DeviceBuffer();
			auto vao = new VertexBuffer(Line);
			vbo->BindData(3 * 2 * sizeof(float), line);
			auto tup = std::tuple<VertexBuffer*, DeviceBuffer*, float*> (vao, vbo, line);

			buffers_.push_back(tup);
		}

	}

	void Draw(glm::mat4 pMVP) override
	{
		program_->Activate();
		auto mvpHandle = program_->GetHandle("MVP");
		auto mvp = pMVP * GetScreenTransform();
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &mvp[0][0]);
		auto colorHandle = program_->GetHandle("nColor");


		glLineWidth(3.f);
		glm::vec3 color;
		for(int i = 0; i < buffers_.size(); ++i)
		{
			auto buffer = buffers_[i];
			auto vao = std::get<0>(buffer);
			auto vbo = std::get<1>(buffer);

			vao->Bind();
			vao->BindAttrib(vbo, 0, 3);
			color = i == 0 ? Red() : i == 1 ? Green() : Blue();
			glUniform3fv(colorHandle, 1, &color[0]);
			vao->Draw(3 * 2);
		}
		glLineWidth(1.f);
	}
protected:
	std::vector<std::tuple<VertexBuffer*, DeviceBuffer*, float*> > buffers_;
	ShaderProgram* program_;
};

#endif