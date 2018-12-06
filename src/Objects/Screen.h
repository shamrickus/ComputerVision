#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "Object.h"
#include "../Shaders/ShaderProgram.h"
#include "../Buffer/DeviceBuffer.h"

const char* v_ortho_texture = 
"#version 330\n" 
"in vec3 vPoints_M;\n" 
"in vec2 vUV;\n"
"uniform mat4 MVP;\n" 
"out vec2 uv;\n"
"void main() { \n" 
	"gl_Position = MVP * vec4(vPoints_M.xz, 0.0, 1.0);\n" 
	"uv = vUV;\n"
"}\n" ;
const char* f_ortho_texture =
"#version 330\n"
"in vec2 uv;\n"
"uniform sampler2D texture;\n"
"out vec4 color;\n"
"void main() {\n"
	"color = texture2D(texture, uv);\n"
"}\n";
class Screen : public Object
{
public:
	Screen(glm::vec3 pBase, glm::vec3 pSize) : Object(pBase)
	{
		size_ = pSize;
		points_ = new float[18]
		{
			0,0,0,
			1,0,0,
			1,0,1,

			1,0,1,
			0,0,1,
			0,0,0
			/*
			base_.x, base_.y, base_.z,
			base_.x+size_.x, base_.y, base_.z,
			base_.x+size_.x, base_.y, base_.z + size_.z,

			base_.x+size_.x, base_.y, base_.z + size_.z,
			base_.x, base_.y, base_.z + size_.z,
			base_.x, base_.y, base_.z,
		*/
		};
		uv_ = new float[12]
		{
			0, 0,
			1, 0,
			1, 1,
			1, 1,
			0, 1,
			0, 0,
		};

		vao_ = new VertexBuffer();
		vao_->Bind();
		vbo_ = new DeviceBuffer();
		vbo_->BindData(18 * sizeof(float), points_);
		texBo_ = new DeviceBuffer();
		texBo_->BindData(12 * sizeof(float), uv_);

		projShader_ = new ShaderProgram();
		auto vs = new Shader(v_texture, ShaderType::Vertex);
		auto fs = new Shader(f_texture, ShaderType::Fragment);
		projShader_->AttachShader(vs);
		projShader_->AttachShader(fs);
		projShader_->Link();

		orthoShader_ = new ShaderProgram();
		auto vs1 = new Shader(v_ortho_texture, ShaderType::Vertex);
		auto fs1 = new Shader(f_ortho_texture, ShaderType::Fragment);
		orthoShader_->AttachShader(vs1);
		orthoShader_->AttachShader(fs1);
		orthoShader_->Link();

		glErrorCheck(__LINE__, __FILE__);
		assert(vbo_->Validate());
		assert(texBo_->Validate());
		assert(vao_->Validate());
	}

	void SetTexture(GLuint pTex) { tex_ = pTex; }

	void Draw(glm::mat4 pMVP, bool pOrtho)
	{
		assert(glIsTexture(tex_));
		assert(vao_->Validate());
		vao_->Bind();
		ShaderProgram* program = pOrtho ? orthoShader_ : projShader_;
		program->Activate();
		auto mvpHandle = program->GetHandle("MVP");
		auto mvp = pMVP * GetScreenTransform();
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, &mvp[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_);
		glUniform1i(program->GetHandle("texture"), 0);

		vao_->BindAttrib(vbo_, 0, 3);
		vao_->BindAttrib(texBo_, 1, 2);

		vao_->Draw(18);
		glErrorCheck(__LINE__, __FILE__);
	}
private:
	DeviceBuffer * vbo_, *texBo_;
	VertexBuffer* vao_, *texVo_;
	float* points_, *uv_;
	ShaderProgram* projShader_, *orthoShader_;
	GLuint tex_;
};

#endif