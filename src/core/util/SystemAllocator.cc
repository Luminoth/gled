#include "src/pch.h"
#include "SystemAllocator.h"

namespace energonsoftware {

Logger& SystemAllocator::logger(Logger::instance("gled.core.util.SystemAllocator"));

SystemAllocator::SystemAllocator(size_t size)
    : MemoryAllocator(), _size(size), _used(0)
{
}

SystemAllocator::~SystemAllocator() throw()
{
}

void* SystemAllocator::allocate(size_t bytes)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    assert(_used + bytes < _size);

    _allocation_count++;
    _allocation_bytes += bytes;

    _used += bytes;
    unsigned char* r = new unsigned char[bytes];

    //LOG_DEBUG("Allocating memory at " << reinterpret_cast<void*>(r) << "\n");
    return r;
}

void SystemAllocator::release(void* ptr)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    delete[] reinterpret_cast<unsigned char*>(ptr);
}

}
