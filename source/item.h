#pragma once

#include "nsmb.h"
#include "nsbtx.hpp"

class ExtraItem : public StageEntity
{
public:

	virtual s32 onCreate() override;
	virtual s32 onUpdate() override;
	virtual s32 onRender() override;
	virtual s32 onDestroy() override;

	static void activeColliderCallback(ActiveCollider& self, ActiveCollider& other);

	static bool loadResources();

	static constexpr u16 objectID = 0x14A;

	static const ActorProfile profile;
	static constexpr u16 updatePriority = objectID;
	static constexpr u16 renderPriority = 0x0000;
	
private:
	
	Model model;
	PlayerBase* player;
};
