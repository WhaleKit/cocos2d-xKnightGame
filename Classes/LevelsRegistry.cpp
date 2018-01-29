#include "LevelsRegistry.h"

namespace {
LevelsRegistry* instance=nullptr;
}

LevelsRegistry *LevelsRegistry::getInstance()
{
    if(instance == nullptr) {
        instance = new LevelsRegistry{};
    }
    return instance;
}
cocos2d::TMXTiledMap* LevelsRegistry::getMap(const std::string &name)
{
    return cocos2d::TMXTiledMap::create(mapsFolder+name);
}
std::vector<std::string> LevelsRegistry::mapNames()
{
    auto fu = cocos2d::FileUtils::getInstance();
    std::vector<cocos2d::Value> vals = fu->getValueVectorFromFile(mapsFolder+"mapList.txt");
    std::vector<std::string> result;
    for (cocos2d::Value& val :vals){
        if(val.getType() == cocos2d::Value::Type::STRING){
            result.push_back(std::move(val).asString());
        }
    }
    return result;
}

std::string LevelsRegistry::getMapsFolder() const
{
    return mapsFolder;
}

void LevelsRegistry::setMapsFolder(const std::string &value)
{
    mapsFolder = value;
}
