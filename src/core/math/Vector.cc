#include "src/pch.h"
#include "Vector.h"

namespace energonsoftware {

/*void Vector::destroy(Vector* const vector, MemoryAllocator* const allocator)
{
    vector->~Vector();
    operator delete(vector, 16, *allocator);
}*/

Vector* Vector::create_array(size_t count, MemoryAllocator& allocator)
{
    Vector* vectors = reinterpret_cast<Vector*>(allocator.allocate(sizeof(Vector) * count, 16));

    Vector *vector = vectors, *end = vectors + count;
    while(vector != end) {
        new(vector) Vector();
        vector++;
    }

    return vectors;
}

void Vector::destroy_array(Vector* const vectors, size_t count, MemoryAllocator* const allocator)
{
    Vector* vector = vectors + count;
    while(vector > vectors) {
        (--vector)->~Vector();
    }
    operator delete[](vectors, 16, *allocator);
}

std::string Vector::str() const
{
    std::stringstream ss;
    ss << "Vector(x:" << std::fixed << x() << ", y:" << y() << ", z:" << z() << ", w:" << w() << ")";
    return ss.str();
}

}