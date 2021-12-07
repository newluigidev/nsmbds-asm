#include "nsmb.h"
#include "nsmb/stage/actors/item.h"

// Item class hooks; ConsumeItemResult enum written by ItzTacos

enum class ConsumeItemResult
{
	None,
	Points1000,
	OneUp,
	Destroy
};

static u8* playerNum = (u8*)0x02085A7C;
static int updateStep;
static bool isCollected;
static bool setRuntime;

nodisc static void loadResources()
{
	FS::Cache::loadFile(2091 - 131, false); //loads Iceflower model from z_files folder
	return;
}

hook(0x020D5050, 10) // Item::loadResources
asm_func static void loadResourcesHook()
{asm(R"(
	BL		_ZL13loadResourcesv
	MOV		R0, #1				@ replaced instruction
	B		0x020D5054
)");}

nodisc static ConsumeItemResult consumeYourItem(Item& item, PowerupState currentPowerup)
{
	if (setRuntime == false && (currentPowerup != PowerupState::Mega))
	{
		item.collidedPlayer->powerupChangeStep = 1; //never actually figured out how this properly works - it doesn't work with this item since Mario's palette 'flicker' effect is set through the fireflower change code
		item.collidedPlayer->powerupScaleTimer = 10; //does the scaling effect (this does work correctly :P)
		
		isCollected = true;
	
		item.collidedPlayer->runtimePowerup = PowerupState::Fire; 
		//Iceflower is a hacked fireflower - plan was to make it a new powerup, but that process is quite complicated due to the jumbled nature of the game's code.
		
	}
	
	else if (setRuntime == true)
		Sound::playSFXChannel0(0x17D, 0);
	
	return ConsumeItemResult::Points1000;
}

nodisc static bool updateYourItem(Item& item)
{
	if (item.updateStep == -1)
	{
		return true;
	}

	if (!item.updateStep)
	{
		updateStep = 1;
		item.renderBlock = true; //since blocks are tiles, models will always render ontop of them
		//therefore the game has a model used block which allows for the illusion that the item appears behind the block 
		
		//controls the "item out" state
		
		if (item.blockHitMode == 0 || item.blockHitMode == 1 || item.blockHitMode == 2)
			item.velocity.y = 0.4fx; //if block is hit from below or side 
		if (item.blockHitMode == 3)
			item.velocity.y = -0.4fx; //if block is hit from the top 
		
		item.updateStep++;
		return true;
	}

	item.applyMovement();
	
	if (!Math::inRange(item.position.y, item.blockPosition.y - 8.0fx, item.blockPosition.y + 8.0fx) && updateStep == 1) //checks if item is out of block 
		updateStep = 2;
	
	
	// if item is out this code is ran 
	if (updateStep == 2)
	{
		item.updateVerticalVelocity(); //updates gravity 
		item.updateCollisionSensors(); //updates the tile/solid collision 
		item.activeCollider.link(); //links the actor collision 
		item.checkConsume(); //activates the check for player-item collision 
		item.renderBlock = false; //stops rendering the model used block 
	}
	
	return true;
}


nodisc static void setupYourItem(Item& item)
{
	item.item = ItemType::Unused0; //the Iceflower is its own item within the Item class - this is the only way to make it properly come out of blocks 

	item.switchState(ptmf_cast(updateYourItem));
	
	void* modelFile = FS::Cache::getFile(2091 - 131);
	item.itemModel.create(modelFile, nullptr, 0, 0, 0); //Iceflower model
	
	void* blockModelFile = FS::Cache::getFile(1451 - 131);
	item.blockModel.create(blockModelFile, 0, 0); //used block model 
	
	NNS_G3dMdlUseMdlAmb(item.blockModel.data);
	NNS_G3dMdlUseMdlEmi(item.blockModel.data);
	
	item.blockPosition = Vec2(item.position.x, item.position.y + 8.0fx); //sets the used block model to the position of the tile block that was hit
	
	item.scale = Vec3(0x9C0); 
	
	isCollected = false;
}

hook(0x020D1640, 10) // Item::onCreate
asm_func static void onCreateHook()
{asm(R"(
	LDR		R0, [R4, #0x574]	@ replaced instruction
	CMP		R0, #6				@ SpawnItem::Unused0
	BEQ		setupYourItem
	B		0x020D1644
setupYourItem:
	MOV		R0,	R4
	BL		_ZL13setupYourItemR4Item
	B		0x020D1BC0
)");}


hook(0x020D0EB0, 10) // Item::onRender
asm_func static void onRenderHook()
{asm(R"(
	CMP		R1, #9				@ ItemType::Unused0
	BEQ		renderYourItem
	CMP		R1, #4				@ replaced instruction
	B		0x020D0EB4
renderYourItem:
	B		0x020D0EB8
)");}


hook(0x020D13FC, 10) // Item::onUpdate
asm_func static void destroyInactiveFix()
{asm(R"(
	CMP		R0, #9				@ ItemType::Unused0
	BEQ		0x020D1408
	MOV		R0, R4				@ replaced instruction
	B		0x020D1400
)");}

hook(0x020D4884, 10) // Item::checkConsume
asm_func static void checkConsumeHook()
{asm(R"(
	CMP		R1, #6				@ SpawnItem::Unused0
	BEQ		consumeYourItem
	CMP		R1, #5				@ replaced instruction
	B		0x020D4888
consumeYourItem:
	MOV		R0, R8
	MOV		R1, R4
	BL		_ZL15consumeYourItemR4Item12PowerupState
	MOV		R5, R0
	B		0x020D4A14
)");}

quik(0x020FCAD4, 10, "MOV R5, R0", playerHook) //hooks to Player::onRender
void playerHook(Player* player)
{	
	if (isCollected == true)
	{
		player->textureID = 2; //sets the Player's texture to the unused one 
		
		setRuntime = true;
		
		if (player->getFireKeyPressed())
		{
			player->setMovementState(&Player::fireballState);
		}
	}
	
	if ((player->runtimePowerup == PowerupState::Small) || (player->runtimePowerup == PowerupState::Super) || (player->runtimePowerup == PowerupState::Mega) || (player->runtimePowerup == PowerupState::Mini) 
		|| (player->runtimePowerup == PowerupState::Shell) && setRuntime == true)
	{
		player->textureID = 0;
		isCollected = false;
		setRuntime = false;
	}
			
}

rlnk(0x02110F1C, 10)
void repl_02110F1C_ov_0A(int r0, Vec3 r1, char r2) //replaces the instruction that originally spawns the fireball
{
	if (isCollected == true) //if the Iceflower is active, then the Iceball actor is spawned
	{
		if (r2 == 1) // if player is facing left 
			Actor::spawnActor(247, 0, &r1, 0, 0, 0);
		else //facing right 
			Actor::spawnActor(247, 0x00000100, &r1, 0, 0, 0);
	}
	else //restored original function 
		SpawnPlayerFireball(*playerNum, &r1, r2);
}

/*rlnk(0x0209C1BC, 0) 
void repl_0209E440_ov_0(int r0, int r1, Vec3 r2)
{
	Actor::spawnActor(253, 0x00000000, &r2, 0, 0, 0);
	return;
}*/

//above is unused code which hacked the unused Boo block to spawn the Iceflower, which was originally its own actor. 
//It can still be spawned using actor 253, however just turns Mario into Fire Mario.

//over_asm(0x020D0468, 10, "MOV R0, #0x174") //Boo block SFX change 
over_asm(0x020FD2BC, 10, "nop") //related to player movement, forget what it does exactly 


over(0x0212E538, 11) u16 a = 0x40; //changes the player head palette index to the extended palette 
over(0x0212E53A, 11) u16 b = 0x20; //fixes the starman head palette index 
over(0x0212E5B0, 11) u8 c = 4; //changes the cap palette to the extended palette 
over_asm(0x0212D308, 11, "LDR R0, =0x0212E544") //according to Newer DS, this fixes the cap palette when the player is killed by lava 