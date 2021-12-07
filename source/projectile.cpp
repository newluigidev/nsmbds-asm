#include "projectile.h"

constexpr ActorProfile Projectile::profile = {&constructObject<Projectile>, updatePriority, renderPriority, loadResources};

over(0x02039D58) const ActorProfile* ProjectileProfile = &Projectile::profile;

static ActiveColliderInfo collisionInfo = {
	0, 
	0x8000, 
	0x7000,
	0x8000,
	
	CollisionGroup::Fireball, 
	CollisionFlag::Fireball, 
	MAKE_GROUP_MASK(CollisionGroup::Entity, CollisionGroup::Hostile, CollisionGroup::Bullet), // groups it collides with
    MAKE_FLAG_MASK(CollisionFlag::None), // other colliders with flags it collides with
	
	0,
	Projectile::activeColliderCallback
};

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

static PointSensor sideSensor = {
	0, 
	0x8000,
	0x8000,
};

bool Projectile::loadResources()
{
	FS::Cache::loadFile(2089 - 131, false); //loads Iceball bitmap 
	return 1;
}

s32 Projectile::onCreate()
{
	nsbtx.setup(FS::Cache::getFile(2089 - 131),  {16, 16}, {8, 8}); //sets up Iceball bitmap for rendering 
	renderSize = {1.0fx, 1.0fx}; //increases the distance that the Iceball can render away from the player 
	
	activeCollider.init(this, collisionInfo, 0);
	activeCollider.link();
	
	collisionMgr.init(this, &botSensor, &topSensor, &sideSensor, 0);
	
	position.x -= 8.0fx;
	
	player = Game::getPlayer(0);
	
	return 1;
}

s32 Projectile::onUpdate() 
{
	this->updateVerticalVelocity();
	this->updateCollisionSensors();
	this->applyMovement();
	
	spawnPos = {(position.x), (position.y + 0x10000), (position.z)};
	
	//sets the Iceball to a constant speed depending on the direction of Mario 
	//if player is facing left 
	if (((settings >> 8) & 0x1) == 1)
	{
		velocity.x = 2.0fx;
	}
	
	//if player is facing right 
	else 
	{
		velocity.x = -2.0fx;
	}
	
	
	//checks for collision with a wall - deletes if collision occurs 
	if (bool((collisionMgr.collisionResult & CollisionMgr::CollisionResult::WallAny)))
	{	
		SpawnParticle(0x95, &spawnPos);
	
		Sound::playSFXChannel0(0x15A, 0);
		this->destroy(true);
	}
	
	
	//checks for collision with the ground - vertical speed is increased in order to make the Iceball bounce 
	if (bool(collisionMgr.collisionResult & CollisionMgr::CollisionResult::GroundAny))
    {
        velocity.y = 3.0fx;
    }
	
	int xoffs = position.x - player->position.x;
	int yoffs = position.y - player->position.y;

	if (xoffs > 100.0fx || xoffs < -100.0fx || yoffs > 100.0fx || yoffs < -100.0fx) //from Newer DS - checks for the distance from the player for both X & Y; deletes if further then 10 tiles 
		this->destroy(true);
	
	return 1;
}

s32 Projectile::onRender()
{
	Vec3 prjdrawPos = {(position.x - 8.0fx), position.y, position.z};
	nsbtx.draw(prjdrawPos, this->scale);
	return 1;
}

s32 Projectile::onDestroy()
{
	return 1;
}

void Projectile::activeColliderCallback(ActiveCollider& self, ActiveCollider& other)
{
	if (other.owner->actorType == ActorType::Player)
			return;
		
	StageEntity* actor = (StageEntity*)other.owner;
	
	Projectile* prj = (Projectile*)self.owner;
	
	Vec3 iceblockPos = {actor->position.x, actor->position.y, actor->position.z};
	
	SpawnParticle(0x95, &prj->spawnPos);
	
	Sound::playSFXChannel0(0x15A, 0);
	
	//so bad, but its really the only way to check for multiple actors & multiple conditions
	//a switch statement would probably look just as bad 
	//anyway decides what type of Iceblock to use on certain actors 
	
	if (actor->id == 35 || actor->id == 83 || actor->id == 40 || actor->id == 54 ||
		actor->id == 28 || actor->id == 84 || actor->id == 90 || actor->id == 136 || actor->id == 147 || actor->id == 143 || actor->id == 145 
		|| actor->id == 123 || actor->id == 148 || actor->id == 149 || actor->id == 30 || actor->id == 126 || actor->id == 127 || actor->id == 110 || 
		actor->id == 89 || actor->id == 118 || actor->id == 107)
		Actor::spawnActor(251, 0, &iceblockPos, 0, 0, 0); 
		
	if (actor->id == 94 || actor->id == 95 || actor->id == 96 || actor->id == 68 || actor->id == 69 || actor->id == 88 || actor->id == 89
		|| actor->id == 55 || actor->id == 39)
		Actor::spawnActor(251, 0x00000100, &iceblockPos, 0, 0, 0);
	
	if (actor->id == 23 || actor->id == 24 || actor->id == 25 || actor->id == 26 || actor->id == 99 || actor->id == 101 || actor->id == 70
		|| actor->id == 155 || actor->id == 120 || actor->id == 56 || actor->id == 117 || actor->id == 91)
		Actor::spawnActor(251, 0x00000200, &iceblockPos, 0, 0, 0);
		
	if (actor->id == 74 || actor->id == 81)
		Actor::spawnActor(251, 0x00000300, &iceblockPos, 0, 0, 0);
	
	if (actor->id == 251 || actor->id == 253 || actor->id == 247 || actor->id == 100 || actor->id == 102 || actor->id == 21)
		return;
	
	if (actor->id == 37 || actor->id == 38 || actor->id == 75 || actor->id == 76 || actor->id == 73 || actor->id == 112 || actor->id == 29
		|| actor->id == 150|| actor->id == 98)
		self.owner->destroy();
	else 
	{
		self.owner->destroy();
		actor->destroy(true);
	}
		
		
}