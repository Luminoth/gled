#include "src/pch.h"
#include "State.h"

namespace energonsoftware {

Logger& State::logger(Logger::instance("gled.engine.State"));

void State::destroy(State* const state, MemoryAllocator* const allocator)
{
    state->~State();
    operator delete(state, *allocator);
}

State::State()
{
}

State::~State() throw()
{
}

bool State::load_font(const std::string& name, size_t height, const Color& color)
{
    if(!_font.load(name, height)) {
        return false;
    }
    _font.color(color);
    return true;
}

}
