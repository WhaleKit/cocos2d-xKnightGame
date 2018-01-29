#ifndef __NON_PLAYER_CHARACTER_H__
#define __NON_PLAYER_CHARACTER_H__

#include "cocos2d.h"
//#include "GameLevel.h"
class GameLevel;

class NonPlayerCharacter : public cocos2d::Node
{
public:
    /**
     * function that is called each turn after player's move
     * so that NPC can make a move
     * @brief
     * @param map
     * @return action that moves a piece when invoked
     */
    virtual
    cocos2d::ActionInterval* makeAMove(GameLevel const& map) =0;
    /**
     * @brief coordsCanCaptureAt
     * @param map
     * @return vector of coordinates this piece could capture enemy standing at
     */
    virtual
    std::vector<cocos2d::Vec2> coordsCanCaptureAt (GameLevel const& map)=0;
    /**
     * @brief coordsCanMoveTo
     * @param map
     * @return vector of coordinates this piece can move to
     */
    virtual
    std::vector<cocos2d::Vec2> coordsCanMoveTo (GameLevel const& map)=0;

    /**
     * initiative of npc determines order in which NPCs move. NPC with higher iniative move first
     * @brief get initiative of this NPC
     * @return initiative of this NPC
     */
    virtual
    float getInitiative()
    {
        return 1.;
    }

    /**
     * @brief removes it's canMoveTo and canCaptureAt markers from @ref map
     * @param map MameLevel from which to remove markers
     * expected to be used by GameLevel
     */
    void removeAllMarkers(GameLevel &map);
    std::vector<cocos2d::Node*> m_canMoveToMarkers;
    std::vector<cocos2d::Node*> m_canCaptureAtMarkers;

    virtual cocos2d::Vec2 getMapCoord() const;
/*
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameLevel);
    */
};

#endif // __NON_PLAYER_CHARACTER_H__
