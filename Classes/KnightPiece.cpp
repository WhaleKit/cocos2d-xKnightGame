#include "KnightPiece.h"

#include "GameLevel.h"

using namespace cocos2d;

KnightPiece *KnightPiece::create(cocos2d::Size size)
{
    KnightPiece* result = new(std::nothrow) KnightPiece;
    auto sprite = cocos2d::Sprite::create("knight.png");
    if (!result){
        return nullptr;
    }
    if(sprite){
        sprite->getTexture()->setAntiAliasTexParameters();
        MakeFitInsideASize(sprite, size);
        result->addChild(sprite);
    }else{
        //put a placeholder if img not found
        auto rectangle = DrawNode::create(4);
        rectangle->drawRect(Vec2(0,0), Vec2(size.width,size.height), Color4F::WHITE);
        auto txt = Label::create("PCkn", "/fonts/arial.ttf", 24, size);
        MakeFitInsideASize(txt, size);
        result->addChild(rectangle, 0);
        result->addChild(txt, 1);
    }
    PutAnchorInCenter(result);
    result->autorelease();
    return result;
}

std::vector<Vec2> KnightPiece::placesHeCanMoveTo(const GameLevel &map)
{
    std::vector<Vec2> result;
    result.reserve(4);
    static const Vec2 arr[8] = {
        Vec2(-2,-1),    Vec2(-2, 1),
        Vec2( 2,-1),    Vec2( 2, 1),

        Vec2(-1,-2),    Vec2(-1, 2),
        Vec2( 1,-2),    Vec2( 1, 2),
    };//should be constexpr, but Vec2 isn't constexpr-constructible for some reason
    for (Vec2 vec : arr) {
        if (map.isTileWalkable(vec+getMapCoord(map))) {
            result.push_back(vec+getMapCoord(map));
        }
    }
    return result;
}
Node* KnightPiece::makeMoveToMarker(Vec2 tileCoords, GameLevel &map, std::function<void()> whatsNext)
{
    Sprite* spr = Sprite::create("gotoMarker.png");
    spr->setName(
        std::string("moveMarkerAt(") +
                std::to_string(tileCoords.x) + ", " +
                std::to_string(tileCoords.y) + ")"
    );
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    auto touchListener = EventListenerTouchOneByOne::create();
    {
        spr->setUserObject( new sharedAny{ make_any<Vec2>(tileCoords)} );
    }
    auto touchReaction = [this, &map, tileCoords, spr,  whatsNextFun = std::move(whatsNext)]
            (cocos2d::Touch* t, cocos2d::Event* e) -> bool {
        if(!spr->getBoundingBox().containsPoint(spr->getParent()->convertTouchToNodeSpace(t)) ) {
            //why do I have to do this myself?
            //(and I _do_ have to - don't take my word for it, try removing this "if" blocks and see for youself)
            //upd: maybe it has something to do with the fact that "spr" is not in scenegraph yet?
            return false;
        }
        auto a0 = this->MoveToTile(tileCoords, map);
        auto a1 = CallFunc::create(whatsNextFun);
        map.runAction(Sequence::createWithTwoActions(a0,a1));
        e->stopPropagation();
        return true;
    }; //POSSIBLE BUG: I hope removing event listener in the event handling code won't cause any problem
    touchListener->onTouchBegan = touchReaction;
    eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, spr);
    MakeFitInsideASize(spr, map.getTileSize());
    PutAnchorInCenter(spr);
    spr->setPosition(map.getTileCenterPosFromCoord(tileCoords));
    this->canMoveToMarkers.push_back(spr);
    return spr;
}

void KnightPiece::notifyHeCantGoToThisMarker(Node* nd)
{
    auto iter = std::find(canMoveToMarkers.begin(), canMoveToMarkers.end(), nd);
    canMoveToMarkers.erase(iter);
}

void KnightPiece::RemoveAllMoveToMarkers(GameLevel &map)
{
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    for (Node* nd:this->canMoveToMarkers) {
        eventDispatcher->removeEventListenersForTarget(nd);
        map.removeMarker(nd);
    }
}

FiniteTimeAction* KnightPiece::MoveToTile(Vec2 tileCoord, GameLevel &map)
{
    //remove all his move-to markers
    auto a0 = CallFuncN::create([&map](Node* target){
            static_cast<KnightPiece*>(target)->RemoveAllMoveToMarkers(map);
    });
    //move the piece
    auto a1 = MoveTo::create(map.moveTime, map.getTileCenterPosFromCoord(tileCoord));

    return  TargetedAction::create(this, Sequence::createWithTwoActions(a0,a1));
}

