#ifndef _CUBE_H_
#define _CUBE_H_

#include "Shaders/ShaderProgram.h"
#include "reactphysics3d.h"

class Cube
{
public:
	Cube(rp3d::DynamicsWorld* pWorld) 
	{
		base_ = glm::vec3(0);
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
		auto fs = new Shader(f_pthr, ShaderType::Fragment);
		shader_->AttachShader(vs);
		shader_->AttachShader(fs);

		shader_->Link();

		boxShape_ = new rp3d::BoxShape(rp3d::Vector3(1.0, 1.0, 1.0));
		transform_ = rp3d::Transform(rp3d::Vector3(base_.x, base_.y, base_.z), rp3d::Quaternion::identity());
		rb_ = pWorld->createRigidBody(transform_);
		rb_->addCollisionShape(boxShape_, transform_, 1);
	}


	void Draw(glm::mat4 pMVP)
	{
		vbo_->Bind();
		shader_->Activate();
		auto mvpHandle = shader_->GetHandle("MVP");
		float pMat[16];
		rb_->getTransform().getOpenGLMatrix(pMat);
		//pMVP *= pMat;
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &pMVP[0][0]);
		vao_->Draw(12 * 3);
	}

	~Cube()
	{
		delete points_;
		delete shader_;
		delete vao_;
		delete vbo_;
		delete boxShape_;
		delete rb_;
	}
private:
	DeviceBuffer * vbo_;
	VertexBuffer* vao_;
	glm::vec3 base_;
	float *points_;
	ShaderProgram* shader_;
	rp3d::RigidBody* rb_;
	rp3d::BoxShape* boxShape_;
	rp3d::Transform transform_;
};

#endif