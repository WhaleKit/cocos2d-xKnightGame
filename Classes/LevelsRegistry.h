#ifndef __LEVELS_REGISTRY_H__
#define __LEVELS_REGISTRY_H__

#include <vector>
#include <string>

#include "cocos2d.h"
class LevelsRegistry
{
public:
    static LevelsRegistry* getInstance();

    LevelsRegistry()
    {

    }
    ~LevelsRegistry()
    {

    }
    cocos2d::TMXTiledMap* getMap(std::string const& name);
    std::vector<std::string> mapNames ();

    std::string getMapsFolder() const;
    void setMapsFolder(const std::string &value);

private:
    std::string mapsFolder = "maps/";
};

#endif // __LEVELS_REGISTRY_H__
