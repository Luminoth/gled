#include "src/pch.h"
#include "Character.h"

namespace energonsoftware {

void Character::destroy(Character* const character, MemoryAllocator* const allocator)
{
    character->~Character();
    operator delete(character, 16, *allocator);
}

Character::Character(const std::string& name)
    : Actor(name)
{
}

Character::~Character() throw()
{
}

}