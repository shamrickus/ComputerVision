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
	
	void SetPosition(glm::vec3 pPos)
	{
		base_ = pPos;
	}

	virtual void Draw(glm::mat4 pMVP)
	{
	}

	virtual void SetSize(glm::vec3 pSize)
	{
		size_ = pSize;
	}

	~Object()
	{
	}
	virtual glm::mat4 GetScreenTransform()
	{
		return GetTranslation() * GetRotation() * GetScale();
	}
	virtual glm::mat4 GetRotation()
	{
		return glm::scale(glm::identity<glm::mat4>(), size_);
	}
	virtual glm::mat4 GetScale()
	{
		return glm::identity<glm::mat4>();
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