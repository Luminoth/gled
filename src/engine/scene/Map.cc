#include "src/pch.h"
#include "src/core/common.h"
#include "src/engine/Engine.h"
#include "src/engine/ResourceManager.h"
#include "src/engine/State.h"
#include "Map.h"

namespace energonsoftware {

const size_t Map::MAX_LIGHTS = 8;
Logger& Map::logger(Logger::instance("gled.engine.scene.Map"));

Map::Map(const std::string& name)
    : _name(name)
{
}

Map::~Map() throw()
{
}

void Map::unload()
{
    _lights.clear();

    on_unload();
}

bool Map::load_material(const std::string& name)
{
    _material = Engine::instance().resource_manager().material(name);
    return static_cast<bool>(_material);
}

}
