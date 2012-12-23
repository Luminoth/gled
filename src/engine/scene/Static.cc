#include "src/pch.h"
#include "src/engine/State.h"
#include "Static.h"

namespace energonsoftware {

void Static::destroy(Static* const sttic, MemoryAllocator* const allocator)
{
    sttic->~Static();
    operator delete(sttic, 16, *allocator);
}

Static::Static(const std::string& name)
    : Renderable(name)
{
}

Static::~Static() throw()
{
}

void Static::on_render_unlit(const Camera& camera) const
{
    const AABB& bounds(absolute_bounds());
    const Position& center(bounds.center());
    _nameplate.render(name(), Vector3(center.x(), bounds.maximum().y(), center.z()), camera);
}

}