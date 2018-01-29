#include "GameLevel.h"

#include <unordered_set>

#include "KnightPiece.h"
#include "levelSelectionScreen.h"

using namespace cocos2d;
namespace {


Vec2 TmxObjectcCoord (ValueMap const& obj, TMXTiledMap const& map)
{
    /*
     * In debugger:
     * Expressions
        map.getTileSize()	@0x335280	cocos2d::Size &
            ZERO	@0x6dc660	cocos2d::Size
            height	-0.00132703932468	float
            width	0.0	float
        map._tileSize	@0x335278	cocos2d::Size
            ZERO	@0x6dc660	cocos2d::Size
            height	32.0	float
            width	32.0	float
    this is... interesting. I gues I'll have to hack around it*/

    float x = obj.at("x").asFloat();
    float y = obj.at("y").asFloat();
    auto const& size = map.getLayer("map")->getTileAt(Vec2(0,0))->getBoundingBox().size;
    x /= size.width;
    y /= size.height;
    return Vec2(std::floor(x), std::floor(y));
}
};


Scene *GameLevel::createSceneWithMap(cocos2d::TMXTiledMap *map)
{
    Scene* result = Scene::create();
    GameLevel* level = new(std::nothrow) GameLevel;
    if (!level){
        return nullptr;
    }
    if (!level->initWithMap(map)) {
        delete level;
        return nullptr;
    }
    MakeFitInsideASize(level, Director::getInstance()->getWinSizeInPixels());
    result->addChild(level);
    result->autorelease();
    return result;
}

bool GameLevel::initWithMap(cocos2d::TMXTiledMap *map)
{

    if (!cocos2d::Layer::init()) {
        return false;
    }
    MakeFitInsideASize(map, Director::getInstance()->getWinSizeInPixels());
    this->markers = Node::create();
    this->addChild(markers, 1);
    this->m_tmxMap = map;
    this->addChild(map,0);
    TMXLayer* ground = map->getLayer("map");
    m_ground = ground;

    {
        auto mapLocalTileSize = ground->getTileAt(Vec2(0,0))->getBoundingBox().size;
        m_tileSize = convertBetweenNodeSpaces(map, this, mapLocalTileSize);
        m_mapSize = ground->getBoundingBox().size;
        m_mapSize.width  = std::floor(m_mapSize.width / mapLocalTileSize.width);
        m_mapSize.height = std::floor(m_mapSize.height / mapLocalTileSize.height);
    }
    TMXObjectGroup* objs = map->getObjectGroup("things");

    { //put player character
        auto startPos = TmxObjectcCoord(objs->getObject("start"), *map);
        //change on introduction of another player characters
        KnightPiece* kp = KnightPiece::create(this->m_tileSize);
        kp->setPosition(this->getTileCenterPosFromCoord(startPos));
        m_playerCharacter=kp;
        this->addChild(kp, 10);
    }
    { //put goal coordinate
        auto goalCoordinate = TmxObjectcCoord(objs->getObject("goal"), *map);
        auto goalSprite = Sprite::create("ladder.png");
        MakeFitInsideASize(goalSprite, this->m_tileSize);
        PutAnchorInCenter(goalSprite);
        goalSprite->setPosition(this->getTileCenterPosFromCoord(goalCoordinate));
        this->addChild(goalSprite,1);
        this->m_goalCoord = goalCoordinate;
    }
    //TODO: npcs, when figure out how to place the on the map

    this->afterTurnCleanup(); //<- to place markers

    return true;
    //TODO: return false if smth goes wrong
}

void GameLevel::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);

}

Vec2 GameLevel::getTileCenterPosFromCoord(Vec2 coord) const
{
    auto size = this->m_tileSize;
    coord.x *= size.width;
    coord.y *= size.height;

    coord.x += size.width / 2;
    coord.y += size.height / 2;
    return coord;
//    auto bb = m_ground->getTileAt(coord)->getBoundingBox();
//    return Vec2(bb.getMidX(), bb.getMidY());
}

Size GameLevel::getTileSize() const
{
    return this->m_tileSize;
}

Vec2 GameLevel::getCoordFromPos(Vec2 coord) const
{
    return Vec2{
        std::floor(coord.x / getTileSize().width),
        std::floor(coord.y / getTileSize().height)
    };
}

void GameLevel::removeMarker(Node *marker)
{
    markers->removeChild(marker);
}

bool GameLevel::winConditionMet() const
{

    bool won = std::floor(m_goalCoord.x) == std::floor(m_playerCharacter->getMapCoord(*this).x) &&
               std::floor(m_goalCoord.y) == std::floor(m_playerCharacter->getMapCoord(*this).y);
    return won;
}

bool GameLevel::isTileWalkable(Vec2 tileCoord) const
{
    tileCoord.y = m_mapSize.width - 1 - tileCoord.y;
    if (tileCoord.x < 0 || tileCoord.y < 0
        || tileCoord.x >= m_mapSize.width || tileCoord.y >= m_mapSize.height) {

        return false;
    }
    return this->m_ground->getTileGIDAt(tileCoord) == 2;
}

void GameLevel::startNpcTurn()
{
    m_turnInProgress = true;

    //sort vetor
    auto comparator = [this](NonPlayerCharacter* npc0,NonPlayerCharacter* npc1) {
        //first - higher initiative
        if (npc0->getInitiative() != npc1->getInitiative()) {
            return npc0->getInitiative() > npc1->getInitiative();
        }
        auto npc0coord = npc0->getMapCoord();
        auto npc1coord = npc1->getMapCoord();
        float lenFromPc0 = (npc0coord - this->m_playerCharacter->getMapCoord(*this)).lengthSquared();
        float lenFromPc1 = (npc1coord - this->m_playerCharacter->getMapCoord(*this)).lengthSquared();
        //second - closest to player character
        if (lenFromPc0 != lenFromPc1) {
            return lenFromPc0 < lenFromPc1;
        }
        //third - lefter on a field
        //fourth- higher on a field
        return std::tie(npc0coord.x, npc0coord.y) < std::tie(npc1coord.x, npc1coord.y);
    };
    std::sort(m_npcVec.begin(), m_npcVec.end(), comparator);

    //start all moves
    duringTurnNpcsMove(0);
}

void GameLevel::duringTurnKillNpc(int toKill)
{
    this->removeChild(m_npcVec[toKill]);
    m_npcVec[toKill] = nullptr;
}

void GameLevel::duringTurnKillNpc(NonPlayerCharacter *npc)
{
    auto iter = std::find(m_npcVec.begin(), m_npcVec.end(), npc);
    if (iter == m_npcVec.end()) { //there is no such npc in vector
        return;
    }
    int npcNum = iter - m_npcVec.begin();
    duringTurnKillNpc(npcNum);
}

void GameLevel::duringTurnNpcsMove(int toMoveIdx)
{
    if(toMoveIdx == m_npcVec.size()) { //all the npc-s made their move
        //now we shall end the turn
        this->afterTurnCleanup();
        return;
    }
    if (m_npcVec[toMoveIdx] == nullptr) { //this NPC was killed in this turn
        //proceed to the next one
        duringTurnNpcsMove(toMoveIdx+1);
        return;
    }
    turnTimeData.npcMakingAMove = toMoveIdx;


    NonPlayerCharacter& npc = *m_npcVec[toMoveIdx];
    //remove it's markers
    npc.removeAllMarkers(*this);

    //start move
    auto moveAction = npc.makeAMove(*this);
    FiniteTimeAction* planForNextMoveAction = CallFunc::create([this, toMoveIdx](){
        this->duringTurnNpcsMove(toMoveIdx+1);
    });
    //so this sequence action will sshedule running another sequence action, that will...
    //until one day toMove+1 == m_npcVec.size() will come true and cycle will end
    this->runAction(
        Sequence::createWithTwoActions(
            moveAction,
            planForNextMoveAction
        )
    );

    //can you call runAction during CallFunc action? I hope that won't invalidate
    //some iterator, which is used to iterate over container with actions somewhere
    //POSSIBLE_BUG TODO put 'yes' here if it won't crash
}

void GameLevel::afterTurnCleanup()
{
    //remove all nullptr-s (killed npc-s) from m_npcVec
    {
        auto iter = std::remove(m_npcVec.begin(), m_npcVec.end(), nullptr);
        m_npcVec.erase(iter, m_npcVec.end());
    }

    if (winConditionMet()) {
        auto youWon = MenuItemImage::create("youWon.png","youWon.png",[](Ref*){
            Director::getInstance()->replaceScene(LevelsSelectionScreen::createScene());
        });
        MakeFitInsideASize(youWon, this->getBoundingBox().size);
        this->addChild(Menu::create(youWon, nullptr));
        return;
    }
    //put new markers on the map
    std::unordered_set<Vec2> byPlayerWalkable;
    {
        std::vector<Vec2> byPlayerWalkableVec = m_playerCharacter->placesHeCanMoveTo(*this);
        byPlayerWalkable.insert(byPlayerWalkableVec.begin(), byPlayerWalkableVec.end());
    }
    for (NonPlayerCharacter* npc : m_npcVec) {
        //make "npc can move here" markers
        for(Vec2 coord: npc->coordsCanMoveTo(*this)) {
            auto marker = Sprite::create("npcGoToMarker.png");
            MakeFitInsideASize(marker, this->getTileSize());
            PutAnchorInCenter(marker);
            marker->setPosition(this->getTileCenterPosFromCoord(coord));
            this->markers->addChild(marker);
            npc->m_canMoveToMarkers.push_back(marker);
        }
        for (Vec2 coord : npc->coordsCanCaptureAt(*this)) {
            byPlayerWalkable.erase(coord); //player character can't go to tile that is under attact
            Node* marker = Sprite::create("npcCaptureAtMarker.png");
            MakeFitInsideASize(marker, this->getTileSize());
            PutAnchorInCenter(marker);
            marker->setPosition(this->getTileCenterPosFromCoord(coord));
            this->markers->addChild(marker);
            npc->m_canCaptureAtMarkers.push_back(marker);
        }
    }
    for (Vec2 coord : byPlayerWalkable) {
        Node* marker = m_playerCharacter->makeMoveToMarker(coord, *this, [this](){
            this->startNpcTurn();
        });
        this->markers->addChild(marker, 5);
    }
    m_turnInProgress = false;
}
