#pragma once

#include "nsmb.h"
#include "nsbtx.hpp"

class IceBlock : public StageEntity
{
public:

	virtual s32 onCreate() override;
	virtual s32 onUpdate() override;
	virtual s32 onRender() override;
	virtual s32 onDestroy() override;

	static bool loadResources();
	
	static void activeColliderCallback(ActiveCollider& self, ActiveCollider& other);
	
	static void topCollCheck(StageActor& self, StageActor& other);
	
	static void sideCollCheck(StageActor& self, StageActor& other);
	
	virtual void thrown() override;
	
	virtual void grabbed() override;
	
	virtual void onEntityHit() override;

	static constexpr u16 objectID = 0x14C;

	static const ActorProfile profile;
	static constexpr u16 updatePriority = objectID;
	static constexpr u16 renderPriority = 0x0000;

private:

	NSBTX nsbtx;
	Model model;
	Collider collider;
	Player* player;
	
	bool isGrabbed;
};
