#ifndef __KNIGHT_PIECE_H__
#define __KNIGHT_PIECE_H__

#include "cocos2d.h"
#include "PlayerCharacter.h"
#include "WKCocosUtils.hpp"

class KnightPiece : public PlayerCharacter
{
public:

    static
    KnightPiece* create(cocos2d::Size size);

    std::vector<cocos2d::Vec2> placesHeCanMoveTo(GameLevel const& map) override;

    //maybe move method to PlayerCharacter::makeMoveToMarker?
    cocos2d::Node *makeMoveToMarker(cocos2d::Vec2 tileCoords, GameLevel &map,
                                                 std::function<void()> whatsNext) override;
    virtual
    void notifyHeCantGoToThisMarker (cocos2d::Node*nd) override;

    void RemoveAllMoveToMarkers(GameLevel& map);

    //maybe move method to PlayerCharacter::MoveToTile?
    cocos2d::FiniteTimeAction *MoveToTile(cocos2d::Vec2 tileCoord, GameLevel &map) override;

/*
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameLevel);
    */
};

#endif // __KNIGHT_PIECE_H__
