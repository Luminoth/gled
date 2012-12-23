#if !defined __PICKABLE_H__
#define __PICKABLE_H__

#include "src/core/math/Vector.h"

namespace energonsoftware {

class Pickable
{
private:
    static uint32_t pick_ids;

private:
    static uint32_t next_pick_id();

public:
    Pickable();
    virtual ~Pickable() throw();

public:
    uint32_t pick_id() const { return _pick_id; }
    const Color& pick_color() const { return _pick_color; }

private:
    uint32_t _pick_id;
    Color _pick_color;
};

}

#endif
