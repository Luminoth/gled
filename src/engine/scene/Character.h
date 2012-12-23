#if !defined __CHARACTER_H__
#define __CHARACTER_H__

#include "Actor.h"

namespace energonsoftware {

class Character : public Actor
{
public:
    static void destroy(Character* const character, MemoryAllocator* const allocator);

public:
    explicit Character(const std::string& name);
    virtual ~Character() throw();

private:
    Character();
    DISALLOW_COPY_AND_ASSIGN(Character);
};

}

#endif
