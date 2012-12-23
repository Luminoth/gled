#include "src/pch.h"
#include "src/engine/Engine.h"
#include "Player.h"

const float Player::MOVE_SPEED = 100.0f;

boost::shared_ptr<Player> Player::instance()
{
    // player stored in the system allocator
    energonsoftware::MemoryAllocator& allocator(energonsoftware::Engine::instance().system_allocator());

    static boost::shared_ptr<Player> player;
    if(!player) {
        player.reset(new(16, allocator) Player(), boost::bind(&Player::destroy, _1, &allocator));
    }
    return player;
}

void Player::destroy(Player* const player, energonsoftware::MemoryAllocator* const allocator)
{
    player->~Player();
    operator delete(player, 16, *allocator);
}

Player::Player()
    : energonsoftware::Actor("player")
{
}

Player::~Player() throw()
{
}

bool Player::on_think(double dt)
{
    return true;
}
