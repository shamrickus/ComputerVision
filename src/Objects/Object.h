#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ode/ode.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Helper.h"

class Object
{
public:
	Object(glm::vec3 pBase)
	{
		base_ = pBase;
	}
	
	virtual void SetPosition(glm::vec3 pPos)
	{
		base_ = pPos;
	}
	glm::vec3 GetPosition() {
		return base_;
	}

	virtual void Draw(glm::mat4 pMVP)
	{
	}

	virtual void SetSize(glm::vec3 pSize)
	{
		size_ = pSize;
	}
	virtual glm::vec3 GetSize()
	{
		return size_;
	}

	~Object()
	{
	}
	virtual glm::mat4 GetScreenTransform()
	{
		return GetTranslation() * GetScale() *  GetRotation();
	}
	virtual glm::mat4 GetRotation()
	{
		return glm::identity<glm::mat4>();
	}
	virtual glm::mat4 GetScale()
	{
		return glm::scale(glm::identity<glm::mat4>(), size_);
	}
	virtual glm::mat4 GetTranslation()
	{
		return glm::translate(glm::identity<glm::mat4>(), base_);
	}
private:
protected:


	glm::vec3 base_, size_;
};

#endif