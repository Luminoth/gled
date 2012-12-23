#include "src/pch.h"
#include "Monster.h"

namespace energonsoftware {

void Monster::destroy(Monster* const monster, MemoryAllocator* const allocator)
{
    monster->~Monster();
    operator delete(monster, 16, *allocator);
}

Monster::Monster(const std::string& name)
    : Actor(name)
{
}

Monster::~Monster() throw()
{
}

}