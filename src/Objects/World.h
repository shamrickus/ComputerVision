#ifndef _WORLD_H_
#define _WORLD_H_

void nearC(void * data, dGeomID p1, dGeomID p2);

class World
{
public:
	World(glm::vec3 pGravity)
	{
		dInitODE2(0);
		world_ = dWorldCreate();
		space_ = dSimpleSpaceCreate(0);
		group_ = dJointGroupCreate(0);
		bodies_ = std::vector<dBodyID>();
		dWorldSetGravity(world_, pGravity.x, pGravity.y, pGravity.z);
		dWorldSetCFM(world_, 1e-5);
		dWorldSetLinearDamping(world_, 0.00001);
		dWorldSetAngularDamping(world_, 0.005);
		dWorldSetMaxAngularSpeed(world_, 200);
		dWorldSetContactSurfaceLayer(world_, 0.001);
	}

	~World()
	{
		dSpaceDestroy(space_);
		dWorldDestroy(world_);
		dJointGroupDestroy(group_);
		dCloseODE();
	}

	void AddBody(Object* pBody, bool pDisable, unsigned long pCategoryBits, 
		unsigned long pCollideBits, bool pStatic)
	{
		auto body = pBody->CreateBody(world_, space_);
		if (pDisable)
			dBodyDisable(body);
		if (pStatic)
			dBodySetKinematic(body);
		dGeomSetCategoryBits(pBody->Geom(), pCategoryBits);
		dGeomSetCollideBits(pBody->Geom(), pCollideBits);
		bodies_.push_back(body);
	}

	void Update(float pTimeStep)
	{
		dSpaceCollide(space_, this, &nearC);
		dWorldStep(world_, pTimeStep);
		dJointGroupEmpty(group_);
	}
	dWorldID GetWorld() { return world_; }
	dJointGroupID GetGroup() { return group_; }

private:
	std::vector<dBodyID> bodies_;
	dWorldID world_;
	dSpaceID space_;
	dJointGroupID group_;
};

void nearC(void * data, dGeomID p1, dGeomID p2)
{
	World* world = static_cast<World*>(data);
	auto b1 = dGeomGetBody(p1);
	auto b2 = dGeomGetBody(p2);
	dContact contact;

	if (b1 == 0 && b2 == 0)
		return;

	if(dAreConnected(b1, b2))
		return;

	contact.surface.mode = dContactBounce;
	contact.surface.mu = 5;
	contact.surface.bounce = 0.20;
	contact.surface.bounce_vel = 0.01;
	if (dCollide(p1, p2, 1, &contact.geom, sizeof(dContactGeom)))
	{
		dJointID c = dJointCreateContact(world->GetWorld(), 
			world->GetGroup(), &contact);
		dJointAttach(c, b1, b2);
	}
}

#endif

