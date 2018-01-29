#ifndef __GAME_LEVEL_H__
#define __GAME_LEVEL_H__

#include "cocos2d.h"

#include "LevelsRegistry.h"
#include "NonPlayerCharacter.h"
#include "PlayerCharacter.h"

class GameLevel : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createSceneWithMap(cocos2d::TMXTiledMap *map);
    float moveTime = 0.25;

    cocos2d::Vec2 m_goalCoord;
    PlayerCharacter* m_playerCharacter;
    std::vector<NonPlayerCharacter*> m_npcVec;
    cocos2d::Node* markers;
    cocos2d::TMXLayer* m_ground;
    cocos2d::TMXTiledMap* m_tmxMap;
    cocos2d::Size m_tileSize;
    cocos2d::Size m_mapSize;
    bool m_turnInProgress=false;
    struct {
        /**
         * @brief if m_turnInProgress is true, npcMakingAMove contains index of NPC, currectly making a move
         */
        int npcMakingAMove=0;
    }turnTimeData;

    bool initWithMap(cocos2d::TMXTiledMap* map);

    void menuCloseCallback(cocos2d::Ref* pSender);

    cocos2d::Vec2 getTileCenterPosFromCoord(cocos2d::Vec2 coord) const;
    cocos2d::Size getTileSize() const;
    cocos2d::Vec2 getCoordFromPos(cocos2d::Vec2 coord) const;

    void removeMarker(cocos2d::Node* marker);



    bool winConditionMet() const;

    bool isTileWalkable(cocos2d::Vec2 tileCoord) const;

    void startNpcTurn();
private:
    void duringTurnKillNpc(int toKill);
    void duringTurnKillNpc(NonPlayerCharacter* npc);
    void duringTurnNpcsMove(int toMove);

    void afterTurnCleanup();

    // implement the "static create()" method manually
//    CREATE_FUNC(GameLevel);
};

#endif // __GAME_LEVEL_H__
