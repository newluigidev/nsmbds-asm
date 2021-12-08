#include "item.h"

constexpr ActorProfile ExtraItem::profile = {&constructObject<ExtraItem>, updatePriority, renderPriority, loadResources};

over(0x02039D70) const ActorProfile* ExtraItemProfile = &ExtraItem::profile;

bool collected;

//UNUSED ICEFLOWER ACTOR (253) - REFER TO "iceflower.cpp" FOR PROPER ACTOR 

u8* playerNum = (u8*)0x02085A7C;
int spawnTimer;
bool doSpawnState;
PlayerModel* plymdl = (PlayerModel*)0x0212F868;

static ActiveColliderInfo collisionInfo = {

    0, 
    0x8000, 
    0x7000,
    0x8000,
    
    CollisionGroup::Entity, // your actor's group
	CollisionFlag::None,  // your actor's flag
    MAKE_GROUP_MASK(CollisionGroup::Player, CollisionGroup::Sliding), // groups it collides with
    MAKE_FLAG_MASK(CollisionFlag::None), // other colliders with flags it collides with
	
	0,
    
    ExtraItem::activeColliderCallback,

};

bool ExtraItem::loadResources()
{
	FS::Cache::loadFile(2091 - 131, false);
	return 1;
}

s32 ExtraItem::onCreate()
{
	void* modelFile = FS::Cache::getFile(2091 - 131);
	model.create(modelFile, 0, 0);
	
	activeCollider.init(this, collisionInfo, 0);
	activeCollider.link();
	
	scale = Vec3(0x9C0);
	
	player = Game::getPlayer(0);
	
	position.y -= 0x8000;

	if (((settings >> 8) & 0xF) == 1)
	{
		doSpawnState = true;
		spawnTimer = 0;
	}
	
	else doSpawnState = false;
	return 1;
}

s32 ExtraItem::onUpdate() 
{
	if (((settings >> 8) & 0xF) == 1 && doSpawnState == true)
	{
		if (spawnTimer < 40)
		{
			++spawnTimer;
			position.y += 0x600;
		}
	}
	
	if (spawnTimer == 40)
		doSpawnState = false;
	
	return 1;	
}

s32 ExtraItem::onRender()
{
	GFX::identity(model.matrix);
	GFX::translate(model.matrix, position.x, position.y, position.z);
	GFX::rotate(model.matrix, rotation.x, rotation.y, rotation.z);
	Game::modelMatrix = model.matrix;
	model.render(scale.vecfx());
	
	return 1;
}

s32 ExtraItem::onDestroy()
{
	return 1;
}

void ExtraItem::activeColliderCallback(ActiveCollider& self, ActiveCollider& other)
{
	if (other.owner->actorType != ActorType::Player || (((other.owner->settings >> 8) & 0xF) == 1))
			return;
	
	Player* player = (Player*)other.owner;
	
	if (collected == false && (player->runtimePowerup != PowerupState::Mega))
	{
		player->powerupChangeStep = 1;
		player->powerupScaleTimer = 10;
		
		collected = true;
		
		Sound::playSFXChannel0(355, 0);
	
		player->runtimePowerup = PowerupState::Fire;
	}
	
	else 
	{
		Sound::playSFXChannel0(0x17D, 0);
	}
	
	GiveScoreItemForPlayer(4, *playerNum);
	self.owner->destroy();
}

