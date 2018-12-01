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

	virtual dBodyID CreateBody(dWorldID pWorld, dSpaceID pSpace)
	{
		body_ = dBodyCreate(pWorld);
		dBodySetPosition(body_, base_.x, base_.y, base_.z);
		mass_ = dMass();
		dMassSetBox(&mass_, density_, size_.x, size_.y, size_.z);
		geom_ = dCreateBox(pSpace, size_.x, size_.y, size_.z);
		dBodySetMass(body_, &mass_);
		dGeomSetBody(geom_, body_);
		return body_;
	}

	~Object()
	{
	}
	virtual dGeomID Geom()
	{
		return geom_;
	}
private:
protected:
	glm::mat4 GetTranslation()
	{
		auto loc = dGeomGetPosition(geom_);
		base_.x = loc[0];
		base_.y = loc[1];
		base_.z = loc[2];
		auto rot = dGeomGetRotation(geom_);
		return glm::make_mat4(ODEtoOGL(loc, rot));
	}

	glm::vec3 base_, size_;
	dBodyID body_;
	dMass mass_;
	dReal density_;
	dGeomID geom_;
};

#endif