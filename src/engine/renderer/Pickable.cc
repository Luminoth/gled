#include "src/pch.h"
#include "Pickable.h"

namespace energonsoftware {

uint32_t Pickable::pick_ids = 0;

uint32_t Pickable::next_pick_id()
{
    return ++pick_ids;
}

Pickable::Pickable()
    : _pick_id(next_pick_id())
{
    _pick_color = Color(
        _pick_id & 0x000000ff,
        (_pick_id >> 8) & 0x000000ff,
        (_pick_id >> 16) & 0x000000ff,
        (_pick_id >> 24) & 0x000000ff
    );
}

Pickable::~Pickable() throw()
{
}

}