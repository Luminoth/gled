#if !defined __CAMERA_H__
#define __CAMERA_H__

#include "src/core/physics/Physical.h"

namespace energonsoftware {

class Camera : public Physical
{
public:
    static void destroy(Camera* const camera, MemoryAllocator* const allocator);

private:
    static int check_clipping(const Vector4& clipped);

public:
    Camera();
    virtual ~Camera() throw();

public:
    // attaches the camera to a physical
    // so that the camera will follow the physical around
    bool attached() const { return NULL != _attached; }
    void attach(boost::shared_ptr<Physical> physical) { _attached = physical; }
    void detach() { _attached.reset(); }

    // call every frame to adjust the view
    // matrix based on the camera values
    // also adjusts the camera position and orientation to
    // match what it's attached to (if it's attached),
    // so this needs to be called before referencing the camera position, etc
    void look();

    // returns true if some portion of the world-space
    // bounding box is within the viewing frustum
    bool visible(const AABB& bounds) const;

private:
    boost::shared_ptr<Physical> _attached;

private:
    DISALLOW_COPY_AND_ASSIGN(Camera);
};

}

#endif
