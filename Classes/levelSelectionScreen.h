#ifndef __LEVELS_SELECTOIN_SCREEN_H__
#define __LEVELS_SELECTOIN_SCREEN_H__

#include <vector>
#include <string>

#include "cocos2d.h"
//#include "ui/UIListView.h"
class LevelsSelectionScreen : cocos2d::Layer
{
public:
    LevelsSelectionScreen();
    ~LevelsSelectionScreen();
    static LevelsSelectionScreen* create();
    static cocos2d::Scene* createScene();

    bool init();



    struct {
        //in "screensizes", 1 - full screen width/height
        float indentL = 0.05;
        float indentR = 0.05;
        float indentU = 0.05;
        float indentD = 0.05;

        //in millimeters

        //pixel dencity is not the only factor - another is how far user's eyes from the screen.
        //is the user in from of a pc, on a couch in a few meters from his TV or is he looking at phone in his hands?
        //sadly, I couldn't find anything like "estimatedDistanceToUsersEye" function in cocos
        //so, I'll just assume user is in front of a computer, like I am.
        float fontHeight=4;
        float maxLabelWidth=50;

        //in "font heights"
        float spacingVert= 0.3;
        float spacingHor = 0.3;

    } m_displayData;//anonimous struct to group data
private:
    std::vector<std::string> m_mapFileNames;
    cocos2d::Menu* m_buttons;
    std::size_t m_currentPage=0;
    cocos2d::Vec2 m_pageSize{1.f,1.f}; //why the there is no int-based Vector (like Vec2i) class in cocos2d-x goddamnit

    void recalcPageSize();
    std::size_t pageItemsNum() const;
    std::size_t pagesNum() const;
    void displayPage(size_t pagenum);

    void switchToMap(std::string const&map);

};

#endif // __LEVELS_SELECTOIN_SCREEN_H__
