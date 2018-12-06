#ifndef _GRID_H_
#define _GRID_H_
#include "../Buffer/VertexBuffer.h"
#include "../Buffer/DeviceBuffer.h"
#include "../Shaders/Shader.h"
#include "../Shaders/ShaderProgram.h"

enum Origin {Center, TopLeft};
class Grid : public PhysObject {
public:
	Grid(glm::vec3 pBase) : PhysObject(pBase)
	{
		type_ = Center;
		step_ = 50;
		gridSize_ = 500;

		vao_ = new VertexBuffer(Line);
		vbo_ = new DeviceBuffer();
		Build();
		vao_->Bind();
		vbo_->BindData(PointCount() * 9 * sizeof(float), points_);


		fillB_ = new DeviceBuffer();
		fill_ = new VertexBuffer(Tri);
		float* points = new float[6 * 3];
		auto distance = gridSize_ / 2;
		switch(type_)
		{
		case Center:
			points[0] = base_.x - distance; points[1] = base_.y; points[2] = base_.z - distance;
			points[3] = base_.x - distance; points[4] = base_.y; points[5] = base_.z + distance;
			points[6] = base_.x + distance; points[7] = base_.y; points[8] = base_.z - distance;

			points[9] = base_.x + distance; points[10] = base_.y; points[11] = base_.z + distance;
			points[12] = base_.x - distance; points[13] = base_.y; points[14] = base_.z + distance;
			points[15] = base_.x + distance; points[16] = base_.y; points[17] = base_.z - distance;
			break;
		}
		fill_->Bind();
		fillB_->BindData(6 * 3 * sizeof(float), points);

		auto vs = new Shader(v_passthru, Vertex);
		auto fs = new Shader(f_passthru, Fragment);
		shader_ = new ShaderProgram();
		shader_->AttachShader(vs);
		shader_->AttachShader(fs);
		shader_->Link();

		size_ = glm::vec3(100, 1, 100);
		density_ = 1;
		assert(vao_->Validate());
		assert(fill_->Validate());
	}



	void Build()
	{
		auto count = PointCount() * 3;
		auto stride = count / (gridSize_ / step_ + 1);
		points_ = new float[count];
		for(int i = 0; i < count; i+=stride)
		{
			auto distance = gridSize_ / 2;
			auto step = step_ * i/stride - distance;
			switch(type_)
			{
			case Center:
				points_[i]   = base_.x - distance;
				points_[i+1] = base_.y;
				points_[i+2] = base_.z + step;
				points_[i+3] = base_.x + distance;
				points_[i+4] = base_.y;
				points_[i+5] = base_.z + step;

				points_[i+6]  = base_.x + step;
				points_[i+7]  = base_.y;
				points_[i+8]  = base_.z - distance;
				points_[i+9]  = base_.x + step;
				points_[i+10] = base_.y;
				points_[i+11] = base_.z + distance;
				break;
			default: 
				CVLogger::Log("Unknown Grid Type " + type_);
				break;
			}
		}
	}

	void Draw(glm::mat4 pMVP)
	{
		vao_->Bind();
		shader_->Activate();
		auto mvpHandle = shader_->GetHandle("MVP");
		auto mvp = pMVP * GetScreenTransform();
		auto color = glm::vec3(0.3);
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &mvp[0][0]);
		glUniform3fv(shader_->GetHandle("nColor"), 1, &color[0]);
		vao_->BindAttrib(vbo_, 0, 3);
		vao_->Draw(PointCount());

		fill_->Bind();
		color = glm::vec3(1);
		glUniform3fv(shader_->GetHandle("nColor"), 1, &color[0]);
		fill_->BindAttrib(fillB_, 0, 3);
		fill_->Draw(6 * 3);

		glErrorCheck(__LINE__, __FILE__);

	}

protected:
	int PointCount()
	{
		return (gridSize_ / step_ + 1) * 2 * 2;
	}
	float* points_;
	Origin type_;
	int gridSize_;
	int step_;
	DeviceBuffer* vbo_;
	VertexBuffer* vao_;
	ShaderProgram* shader_;
	VertexBuffer* fill_;
	DeviceBuffer* fillB_;

};

#endif
