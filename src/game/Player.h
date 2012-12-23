#if !defined __PLAYER_H__
#define __PLAYER_H__

#include "src/engine/scene/Actor.h"

class Player : public energonsoftware::Actor
{
public:
    static const float MOVE_SPEED;

public:
    static boost::shared_ptr<Player> instance();
    static void destroy(Player* const player, energonsoftware::MemoryAllocator* const allocator);

public:
    Player();
    virtual ~Player() throw();

private:
    virtual bool on_think(double dt);

private:
    DISALLOW_COPY_AND_ASSIGN(Player);
};

#endif
