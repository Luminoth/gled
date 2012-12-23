#include "src/pch.h"
#include "src/core/physics/AABB.h"
#include "src/engine/Engine.h"
#include "Renderer.h"
#include "Camera.h"

namespace energonsoftware {

void Camera::destroy(Camera* const camera, MemoryAllocator* const allocator)
{
    camera->~Camera();
    operator delete(camera, 16, *allocator);
}

int Camera::check_clipping(const Vector4& clipped)
{
    // 3D Math Primer for Graphics and Game Development, section 16.1.1
    int code = 0;
    if(clipped.x() < -clipped.w()) code |= 0x01;  // left plane
    if(clipped.x() >  clipped.w()) code |= 0x02;  // right plane
    if(clipped.y() < -clipped.w()) code |= 0x04;  // bottom plane
    if(clipped.y() >  clipped.w()) code |= 0x08;  // top plane
    if(clipped.z() < -clipped.w()) code |= 0x10;  // near plane
    if(clipped.z() >  clipped.w()) code |= 0x20;  // far plane
    return code;
}

Camera::Camera()
    : Physical()
{
}

Camera::~Camera() throw()
{
}

void Camera::look()
{
    if(attached()) {
        position(_attached->position());
        view(_attached->view_unrotated());
        up(_attached->up_unrotated());
        orientation(_attached->orientation());
    }
    Engine::instance().renderer().lookat(position(), position() + view(), up());
}

bool Camera::visible(const AABB& bounds) const
{
    const Matrix4 clipping(Engine::instance().renderer().clipping_matrix());

    // need to test against the 8 points of the bounding box
    const Position p1(bounds.minimum().homogeneous_position()),
        p2(bounds.minimum().x(), bounds.minimum().y(), bounds.maximum().z(), 1.0f),
        p3(bounds.minimum().x(), bounds.maximum().y(), bounds.minimum().z(), 1.0f),
        p4(bounds.minimum().x(), bounds.maximum().y(), bounds.maximum().z(), 1.0f),
        p5(bounds.maximum().x(), bounds.minimum().y(), bounds.minimum().z(), 1.0f),
        p6(bounds.maximum().x(), bounds.minimum().y(), bounds.maximum().z(), 1.0f),
        p7(bounds.maximum().x(), bounds.maximum().y(), bounds.minimum().z(), 1.0f),
        p8(bounds.maximum().homogeneous_position());

    // if all points lie outside of at least one of the clipping planes, we can cull the object
    return (check_clipping(clipping * p1) & check_clipping(clipping * p2) & check_clipping(clipping * p3)
        & check_clipping(clipping * p4) & check_clipping(clipping * p5) & check_clipping(clipping * p6)
        & check_clipping(clipping * p7) & check_clipping(clipping * p8)) == 0;
}

}
