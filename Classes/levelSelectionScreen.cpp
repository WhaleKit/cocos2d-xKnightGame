#include "levelSelectionScreen.h"

#include "iostream"
//#include "ui/UIButton.h"

#include "WKCocosUtils.hpp"
#include "GameLevel.h"

using namespace cocos2d;

LevelsSelectionScreen::LevelsSelectionScreen()
{

}

LevelsSelectionScreen::~LevelsSelectionScreen()
{
    
}

LevelsSelectionScreen *LevelsSelectionScreen::create()
{
    LevelsSelectionScreen* res = new(std::nothrow) LevelsSelectionScreen;
    if(!res->init()) {
        return nullptr;
    }
    res->autorelease(); //that's dumb, but I'm doint this project cocos2d-x way
    return res;
}

Scene *LevelsSelectionScreen::createScene()
{
    auto scene = Scene::create();
    if (!scene) {
        return nullptr;
    }
    auto layer = LevelsSelectionScreen::create();
    if (!layer) {
        return nullptr;
    }
    scene->addChild(layer);
    return scene;
}

bool LevelsSelectionScreen::init()
{
    if(!Layer::init()) {
        return false;
    }
    FileUtils* fu = FileUtils::getInstance();
    {
        ValueVector mapNames = fu->getValueVectorFromFile("maps/mapList.txt");
        m_mapFileNames.reserve(mapNames.size());
        for (Value const& val : mapNames) {
            m_mapFileNames.push_back(val.asString());
        }
    }
    {
        m_buttons = Menu::create();
        //vararg list terminated by nullptr
        //this is so C
        m_buttons->setPosition(0,0);
        m_buttons->setAnchorPoint(Vec2{0,0});
        this->addChild(m_buttons,5);

    }m_currentPage=0;
    recalcPageSize();
//    for (int i=0; i<4; ++i) {
//        auto button = MenuItemFont::create("text", [](Ref*){
//            std::cout << "\n"<< "buttonPressed"<<std::endl;
//        });
//        button->setFontNameObj("Arial");
//        button->setFontSizeObj(20);
//        button->setPosition(100,100+20*i);
//        button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
//        m_buttons->addChild(button);

//    }
    displayPage(m_currentPage);




    return true;
}

void LevelsSelectionScreen::recalcPageSize()
{
    Size winSize = Director::getInstance()->getWinSizeInPixels();
    Vec2 result;
    //winSize.width = winSize.width * (m_displayData.indentL + m_displayData.indentR);
    result.x = 1; //for now list will be represented 1d on screen

    result.y = winSize.height - winSize.height*(m_displayData.indentU + m_displayData.indentD);
    float textHeightInPx = mmToPixels(m_displayData.fontHeight);
    result.y /= textHeightInPx + textHeightInPx * m_displayData.spacingVert;
    result.y = std::floor(result.y);
    m_pageSize = result;
}

std::size_t LevelsSelectionScreen::pageItemsNum() const
{
    assert(std::fmod(m_pageSize.x, 1) == 0);
    assert(std::fmod(m_pageSize.y, 1) == 0);
    return m_pageSize.x * m_pageSize.y;
}

std::size_t LevelsSelectionScreen::pagesNum() const
{
    return std::ceil(m_mapFileNames.size() / pageItemsNum());
}


void LevelsSelectionScreen::displayPage(size_t pagenum)
{
    m_buttons->removeAllChildrenWithCleanup(true);

    auto calcPos = [this](Vec2 idx){
        Size winSize = Director::getInstance()->getWinSizeInPixels();
        Vec2 result = idx;
        float fontHeightInPx = mmToPixels(m_displayData.fontHeight);
        result.y *= fontHeightInPx +
                fontHeightInPx * m_displayData.spacingVert;
        result.y += m_displayData.indentU * winSize.height;
        result.y = winSize.height - result.y; //cocos2d-x's coordSys is upside down

        result.x *= m_displayData.maxLabelWidth +
                fontHeightInPx * m_displayData.spacingVert;
        result.x += m_displayData.indentL * winSize.width;
        return result;
    };//will move to method if need somewhere else
    auto addLabelButton = [this,calcPos](std::string text, auto idx, auto onPress) {
        float fontHInPix = std::ceil(mmToPixels(m_displayData.fontHeight));
//        auto label = Label::createWithSystemFont(text, "Arial", fontHInPix/*,
//            Size{mmToPixels(m_displayData.maxLabelWidth),fontHInPix}*/
//        );
//        auto mil = MenuItemLabel::create(label, onPress);
        auto item = MenuItemFont::create(text, onPress);
        item->setFontNameObj("Arial");
        item->setFontSizeObj(fontHInPix);
        item->setPosition(calcPos( idx ));
        item->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        m_buttons->addChild(item);
    };

    size_t buttonsPlaced=0;
    {//"back to game menu" button
        addLabelButton("back",Vec2{0,buttonsPlaced}, [](Ref*) {
            Director::getInstance()->popScene();
        });
    }
    ++buttonsPlaced;

    if (m_currentPage != 0) {
        //draw "previous" button
        addLabelButton("previous", Vec2{0,buttonsPlaced}, [this](Ref*) {
            --(this->m_currentPage);
            this->displayPage(this->m_currentPage);
        });
    }
    ++buttonsPlaced;

    if (int{m_currentPage} < int{pagesNum()} - 1) {
        //draw "next" button
        addLabelButton("next", Vec2{0,buttonsPlaced}, [this](Ref*) {
            --(this->m_currentPage);
            this->displayPage(this->m_currentPage);
        });
    }
    ++buttonsPlaced;

    for (int iRaw = 0; iRaw < int{pageItemsNum()} - buttonsPlaced; ++iRaw) {
        int mapIdx = iRaw + (pageItemsNum()-buttonsPlaced) * pagenum;
        int onPageLabelIdx = iRaw+buttonsPlaced;
        if (mapIdx>=m_mapFileNames.size()) {
            break;
        }
        std::string const& mapName = m_mapFileNames[mapIdx];
        addLabelButton(mapName, Vec2(0, onPageLabelIdx),[this, mapName](Ref*) {
            this->switchToMap(mapName);
        });
    }
}

void LevelsSelectionScreen::switchToMap(const std::string& mapName)
{
    auto d = Director::getInstance();
    auto map = LevelsRegistry::getInstance()->getMap(mapName);
    d->replaceScene(GameLevel::createSceneWithMap(map));
}
