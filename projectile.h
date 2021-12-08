#pragma once

#include "nsmb.h"
#include "nsbtx.hpp"

class Projectile : public StageEntity
{
public:

	virtual s32 onCreate() override;
	virtual s32 onUpdate() override;
	virtual s32 onRender() override;
	virtual s32 onDestroy() override;

	static bool loadResources();
	
	static void activeColliderCallback(ActiveCollider& self, ActiveCollider& other);

	virtual void onStageBeaten(Player& player) override;

	static constexpr u16 objectID = 0x14B;

	static const ActorProfile profile;
	static constexpr u16 updatePriority = objectID;
	static constexpr u16 renderPriority = 0x0000;

private:

	NSBTX nsbtx;
	Player* player;
	Vec3 spawnPos;
};

extern int iceballLimit;
