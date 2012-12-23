#include "src/pch.h"
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "Nameplate.h"

namespace energonsoftware {

Nameplate::Nameplate()
{
}

Nameplate::~Nameplate() throw()
{
}

void Nameplate::render(const std::string& name, const Position& position, const Camera& camera) const
{
    // render the name centered on top of us
    Engine::instance().state().font().render(name, position, Vector2(1.0f, 1.0f), true);
}

}