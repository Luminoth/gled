#if !defined __STATIC_H__
#define __STATIC_H__

#include "src/engine/renderer/Renderable.h"
#include "Nameplate.h"

namespace energonsoftware {

class Static : public Renderable
{
public:
    static void destroy(Static* const sttic, MemoryAllocator* const allocator);

public:
    explicit Static(const std::string& name);
    virtual ~Static() throw();

public:
    virtual bool is_transparent() const { return false; }
    virtual bool is_static() const { return true; }
    virtual bool has_shadow() const { return true; }

private:
    virtual void on_render_unlit(const Camera& camera) const;

private:
    Nameplate _nameplate;

private:
    Static();
    DISALLOW_COPY_AND_ASSIGN(Static);
};

}

#endif
