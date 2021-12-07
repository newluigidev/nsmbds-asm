#include "iceblock.h"

constexpr ActorProfile IceBlock::profile = {&constructObject<IceBlock>, updatePriority, renderPriority, loadResources};

over(0x02039D68) const ActorProfile* IceBlockProfile = &IceBlock::profile;

static PointSensor topSensor = {
	0, 
	0,
	0x10000
};

static PointSensor botSensor = {
	0, 
	0,
	0
};

static PointSensor topSensor2 = {
	0, 
	0,
	0x1B000
};

static PointSensor botSensor2 = {
	0, 
	0,
	-0x8000
};

static PointSensor botSensor3 = {
	0, 
	0,
	-0x18000
};

static PointSensor sideSensor = {
	0, 
	0x8000,
	0x8000,
};

static ColliderInfo collInfo = {
	
	0,
	0x14000,
	0x14000,
	0,
	
	IceBlock::topCollCheck,
	0,
	IceBlock::sideCollCheck
};

static ActiveColliderInfo collisionInfo = {
	0, 
	0x8000, 
	0x8000,
	0x8000,
	
	CollisionGroup::Entity, 
	CollisionFlag::None, 
	MAKE_GROUP_MASK(CollisionGroup::Entity, CollisionGroup::Hostile, CollisionGroup::Bullet), // groups it collides with
    MAKE_FLAG_MASK(CollisionFlag::None), // other colliders with flags it collides with
	
	0,
	IceBlock::activeColliderCallback
};

bool IceBlock::loadResources()
{
	FS::Cache::loadFile(2090 - 131, false); //loads IceBlock bitmap
	return 1;
}

s32 IceBlock::onCreate()
{
	nsbtx.setup(FS::Cache::getFile(2090 - 131),  {16, 16}, {8, 8}); //setup for displaying the Iceblock bitmap 
	
	//sets up scale of the block and collider settings based on Nybble 10
	
	switch ((settings >> 8) & 0xF)
	{
		case 0:
		{
			collisionMgr.init(this, &botSensor, &topSensor, &sideSensor, 0);
			scale = {0x1400};
			break;
		}
		
		case 1:
		{
			collisionMgr.init(this, &botSensor2, &topSensor2, &sideSensor, 0);
			scale = {0x1400, 0x2800, 0};
			break;
		}
		
		case 2:
		{
			collisionMgr.init(this, &botSensor2, &topSensor2, &sideSensor, 0);
			scale = {0x2800, 0x2800, 0};
			break;
		}
		
		case 3:
		{
			collisionMgr.init(this, &botSensor, &topSensor, &sideSensor, 0);
			scale = {0x2800, 0x1400, 0};
			break;
		}
		default:
			break;
	}
	
	collider.init(this, collInfo, 0, 0);
	collider.link();
	
	activeCollider.init(this, collisionInfo, 0);
	activeCollider.link();
	
	player = Game::getPlayer(0);
	
	return 1;
}

s32 IceBlock::onUpdate() 
{
	this->updateVerticalVelocity();
	this->updateCollisionSensors();
	this->applyMovement();
	collider.updatePosition();
	
	if (bool((collisionMgr.collisionResult & CollisionMgr::CollisionResult::WallAny)) && (isGrabbed == false))
	{	
		SpawnParticle(0xD1, &position);
	
		Sound::playSFXChannel0(0x15A, 0);

		this->destroy(true);
	}
	
	if (isGrabbed == true)
		collider.behavior = 0x20002; //makes the Iceblock non-solid 
	else collider.behavior = 0; //makes it solid again 
	
	
	
	//cout << isGrabbed << "\n";
	
	this->updateGrabbed(); //this member function makes the actor ready to be grabbed
	
	return 1;
}

s32 IceBlock::onRender()
{
	
	//bad place to put this (should be in the update function) - essentially this manually changes the location of the collider based on the size of the Iceblock 
	//never a good idea to change collision during runtime, but this is the only way that it would function reasonably 
	
	switch ((settings >> 8) & 0xF) 
	{
		case 0:
		{
			collider.originPoint = {-0xC000, 0x13000};
			collider.endPoint = {0xC000, 0};
			break;
		}
		case 1:
		{
			collider.originPoint = {-0xC000, 0x1A000};
			collider.endPoint = {0xC000, -0xA000};
			break;
		}
		case 2:
		{
			collider.originPoint = {-0x10000, 0x1A000};
			collider.endPoint = {0x10000, -0xA000};
			break;
		}
		case 3:
		{
			collider.originPoint = {0x1A000, 0xC000};
			collider.endPoint = {-0xA000, -0xC000};
			break;
		}
		default:
			break;
	}
	Vec3 iceblockDrawPos = {(position.x - 0x8000), (position.y), (position.z + 0x220000)};
	nsbtx.draw(iceblockDrawPos, this->scale);
	return 1;
}

s32 IceBlock::onDestroy()
{
	return 1;
}

void IceBlock::activeColliderCallback(ActiveCollider& self, ActiveCollider& other)
{
	return;
}

void IceBlock::topCollCheck(StageActor& self, StageActor& other)
{
	IceBlock& iceblk = static_cast<IceBlock&>(self);
	
	if (other.actorType == ActorType::Player)
	{
		Player& player = static_cast<Player&>(other);
		
		if (player.action.isGroundpounding == 1)
		{
			SpawnParticle(0xD1, &iceblk.position);
			GiveScoreItemForPlayer(4, 0); //1000 points 
			Sound::playSFXChannel0(0x15A, 0); 
			iceblk.destroy(true);
		}
	}	
}

void IceBlock::sideCollCheck(StageActor& self, StageActor& other)
{
	IceBlock& iceblk = static_cast<IceBlock&>(self);
	
	if (other.actorType == ActorType::Player)
	{
		Player& player = static_cast<Player&>(other);
		
		if (((iceblk.settings >> 8) & 0xF) == 0)
		{
			if (iceblk.isGrabbed == false)
			{
				if (player.getCarryKeysHeld())
					iceblk.tryGrab(player);
				
			}
		}
		
		return;
	}	
}

//overriden StageEntity member functions which handle the grabbing, throwing and enemy collision 
//they all call the original functions from StageEntity so that the normal code would run - along with my code which allows for the Iceblock to work correctly when held by the player 

void IceBlock::grabbed()
{
	isGrabbed = true;
	
	StageEntity::grabbed();
}

void IceBlock::thrown()
{
	isGrabbed = false;
	
	StageEntity::thrown();
}

void IceBlock::onEntityHit()
{
	if (isGrabbed == true)
	{
		SpawnParticle(0xD1, &position);
		Sound::playSFXChannel0(0x15A, 0);
		destroy(true);
	}
	
	else StageEntity::onEntityHit();
}