#include "NonPlayerCharacter.h"
#include "GameLevel.h"

using namespace cocos2d;
void NonPlayerCharacter::removeAllMarkers(GameLevel &map)
{
    for (Node* nd : m_canCaptureAtMarkers) {
        map.removeMarker(nd);
    }
    m_canCaptureAtMarkers.clear();
    for (Node* nd : m_canMoveToMarkers) {
        map.removeMarker(nd);
    }
    m_canMoveToMarkers.clear();
}

Vec2 NonPlayerCharacter::getMapCoord() const
{
    return cocos2d::Vec2(std::floor(this->getPositionX())
                         ,std::floor(this->getPositionY()));
}
