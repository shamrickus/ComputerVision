#ifndef _GRID_H_
#define _GRID_H_
#include "Buffer/VertexBuffer.h"
#include "Buffer/DeviceBuffer.h"
#include "Shaders/Shader.h"
#include "Shaders/ShaderProgram.h"

enum Origin {Center, TopLeft};
class Grid{
public:
	Grid(glm::vec3* pOrigin) 
	{
		origin_ = *pOrigin;
		type_ = Center;
		step_ = 10;
		size_ = 100;

		vao_ = new VertexBuffer(Line);
		vbo_ = new DeviceBuffer();
		Build();
		vbo_->BindData(PointCount() * 9 * sizeof(float), points_);
		vao_->Build();

		auto vs = new Shader(v_passthru, Vertex);
		auto fs = new Shader(f_pthr, Fragment);
		shader_ = new ShaderProgram();
		shader_->AttachShader(vs);
		shader_->AttachShader(fs);
		shader_->Link();

		assert(vao_->Validate());
	}

	void Build()
	{
		auto count = PointCount() * 3;
		auto stride = count / (size_ / step_ + 1);
		points_ = new float[count];
		for(int i = 0; i < count; i+=stride)
		{
			auto distance = size_ / 2;
			auto step = step_ * i/stride - distance;
			switch(type_)
			{
			case Center:
				points_[i]   = origin_.x - distance;
				points_[i+1] = origin_.y;
				points_[i+2] = origin_.z + step;
				points_[i+3] = origin_.x + distance;
				points_[i+4] = origin_.y;
				points_[i+5] = origin_.z + step;

				points_[i+6]  = origin_.x + step;
				points_[i+7]  = origin_.y;
				points_[i+8]  = origin_.z - distance;
				points_[i+9]  = origin_.x + step;
				points_[i+10] = origin_.y;
				points_[i+11] = origin_.z + distance;
				break;
			default: 
				Logger::Log("Unknown Grid Type " + type_);
				break;
			}
		}
	}

	void Draw(glm::mat4 pMVP)
	{
		vbo_->Bind();
		shader_->Activate();
		auto mvpHandle = shader_->GetHandle("MVP");
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &pMVP[0][0]);
		vao_->Draw(PointCount());

	}

protected:
	int PointCount()
	{
		return (size_ / step_ + 1) * 2 * 2;
	}
	glm::vec3 origin_;
	float* points_;
	Origin type_;
	int size_;
	int step_;
	DeviceBuffer* vbo_;
	VertexBuffer* vao_;
	ShaderProgram* shader_;

};

#endif