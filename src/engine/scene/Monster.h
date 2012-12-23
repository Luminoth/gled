#if !defined __MONSTER_H__
#define __MONSTER_H__

#include "Actor.h"

namespace energonsoftware {

class Monster : public Actor
{
public:
    static void destroy(Monster* const monster, MemoryAllocator* const allocator);

public:
    explicit Monster(const std::string& name);
    virtual ~Monster() throw();

private:
    Monster();
    DISALLOW_COPY_AND_ASSIGN(Monster);
};

}

#endif
