#ifndef __PLAYER_CHARACTER_H__
#define __PLAYER_CHARACTER_H__

#include "cocos2d.h"
#include "WKCocosUtils.hpp"
class GameLevel;

class PlayerCharacter : public cocos2d::Node
{
    //cocos2d::Vec2 mapCoord;
public:
    std::vector<cocos2d::Node*> canMoveToMarkers;
    virtual
    std::vector<cocos2d::Vec2> placesHeCanMoveTo(GameLevel const& map)=0;

    /**
     * @brief makes marker, that, when pressed, moves this piece to tile at tileCoords (by calling MoveToTile)
     * @param tileCoords  coords to which pressing markes should move piece to
     * @param whatsNext  function that will be called after piece finished moving
     * @return marker, which, when pressed,
     */
    virtual
    cocos2d::Node* makeMoveToMarker(cocos2d::Vec2 tileCoords, GameLevel &map,
                                                 std::function<void()> whatsNext)=0;

    /**
     * @brief used by game level to tell, that playerCharacter can't move to this marker
     * PlayerCharacter should delete it's records of this marker
     */
    virtual
    void notifyHeCantGoToThisMarker (cocos2d::Node*)=0;

    /**
     * @brief make moving action
     * @param tileCoord  coord of tile to which piece should move
     * @return action that will move piece to a tile at tileCoord
     */
    virtual
    cocos2d::FiniteTimeAction* MoveToTile(cocos2d::Vec2 tileCoord, GameLevel& map)=0;
    virtual cocos2d::Vec2 getMapCoord(const GameLevel &map) const;
};

#endif // __PLAYER_CHARACTER_H__
