#include "src/pch.h"
#include "StackAllocator.h"

namespace energonsoftware {

Logger& StackAllocator::logger(Logger::instance("gled.core.util.StackAllocator"));

StackAllocator::StackAllocator(size_t size)
    : MemoryAllocator(), _size(size), _marker(0)
{
    _pool.reset(new unsigned char[_size]);
    //LOG_DEBUG("Pool at " << reinterpret_cast<void*>(_pool.get()) << "\n");
}

StackAllocator::~StackAllocator() throw()
{
}

void* StackAllocator::allocate(size_t bytes)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    assert(_marker + bytes < _size);

    size_t r = _marker;
    _marker += bytes;

    _allocation_count++;
    _allocation_bytes += bytes;

    //LOG_DEBUG("Allocating memory at " << reinterpret_cast<void*>(_pool.get() + r) << "\n");
    return _pool.get() + r;
}

void StackAllocator::release(void* ptr)
{
    // explicitly do nothing here
}

}
