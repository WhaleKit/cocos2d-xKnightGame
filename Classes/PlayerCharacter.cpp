#include "PlayerCharacter.h"
#include "GameLevel.h"

cocos2d::Vec2 PlayerCharacter::getMapCoord(const GameLevel &map) const
{
    return map.getCoordFromPos(this->getPosition());
}
