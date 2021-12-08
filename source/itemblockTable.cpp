#include "nsmb.h"
#include "nsmb/stage/actors/item.h"

//written by ItzTacos 

over(0x020C21E0, 0) const SpawnItem questionBlockSpawnItems[54] = {
    SpawnItem::MushroomFlower,
    SpawnItem::Star,
    SpawnItem::ConditionalStar,
    SpawnItem::OneUp,
    SpawnItem::Springboard,
    SpawnItem::MiniMushroom,
    SpawnItem::FlyingCoins,
    SpawnItem::Coin,
    SpawnItem::Vine,
    SpawnItem::Unused3,
    SpawnItem::Unused6,
    SpawnItem::Empty,
    SpawnItem::Boo,
    SpawnItem::MegaMushroom,
    SpawnItem::Unused0, //Iceflower 
    SpawnItem::None,
    SpawnItem::MushroomFlower,
    SpawnItem::ThrownCoin,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::MushroomFlower,
    SpawnItem::Star,
    SpawnItem::OneUp,
    SpawnItem::Coin,
    SpawnItem::Vine,
    SpawnItem::Unused6
};

over(0x020C20F0, 0) const SpawnItem brickBlockSpawnItems[16] = {
    SpawnItem::None,
    SpawnItem::MushroomFlower,
    SpawnItem::Star,
    SpawnItem::OneUp,
    SpawnItem::Coin,
    SpawnItem::MultiCoin,
    SpawnItem::Vine,
    SpawnItem::TNone,
    SpawnItem::Unused0, //Iceflower
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None,
    SpawnItem::None
};


const u16 commonActors[] = {
    0x15, // Player
    0x1F, // Item
    0x53, // Goomba
    0xF7, // iceball actor
	0xFB, //iceblock actor
    0x181, // END
};

over(0x020A181C, 0) const u16* commonActorsPtr = commonActors;
