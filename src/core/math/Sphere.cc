#include "src/pch.h"
#include "Sphere.h"

namespace energonsoftware {

Sphere::Sphere(const Point3& center, float radius)
    : _center(center), _radius(fabs(radius))
{
}

Sphere::~Sphere() throw()
{
}

}