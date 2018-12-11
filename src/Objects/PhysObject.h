#ifndef _PHYS_OBJECT_H_
#define _PHYS_OBJECT_H_

#include "Object.h"

class PhysObject : public Object
{
public:	
	PhysObject(glm::vec3 pBase) : Object(pBase) {}
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
	virtual dGeomID Geom()
	{
		return geom_;
	}

	virtual void SetSize(glm::vec3 pSize) override
	{
		Object::SetSize(pSize);
	}

	virtual void SetPosition(glm::vec3 pPos) override
	{
		Object::SetPosition(pPos);
		dGeomSetPosition(geom_, pPos[0], pPos[1], pPos[2]);
	}

	void SetRotation(glm::vec3 pRot)
	{
		dMatrix3 r;
		dRFromEulerAngles(r, pRot[0], pRot[1], pRot[2]);
		dGeomSetRotation(geom_, r);
	}

	void SetRotation(dMatrix3 pRot)
	{
		dGeomSetRotation(geom_, pRot);
	}

protected:
	glm::mat4 GetScreenTransform() override
	{
		auto loc = dGeomGetPosition(geom_);
		base_.x = loc[0];
		base_.y = loc[1];
		base_.z = loc[2];
		auto rot = dGeomGetRotation(geom_);
		return glm::make_mat4(ODEtoOGL(loc, rot)) * GetScale();
	}

	dBodyID body_;
	dMass mass_;
	dReal density_;
	dGeomID geom_;
};

#endif