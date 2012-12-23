#include "src/pch.h"
#include "StackAllocator.h"
#include "SystemAllocator.h"
#include "MemoryAllocator.h"

namespace energonsoftware {

#define ARRAY_OFFSET 0x10
//#define ARRAY_OFFSET 0x00

boost::shared_ptr<MemoryAllocator> MemoryAllocator::new_allocator(AllocatorType type, size_t size)
{
    switch(type)
    {
    case AllocatorTypeStack:
        return boost::shared_ptr<MemoryAllocator>(new StackAllocator(size));
    case AllocatorTypeSystem:
        return boost::shared_ptr<MemoryAllocator>(new SystemAllocator(size));
    }
    return boost::shared_ptr<MemoryAllocator>();
}

MemoryAllocator::MemoryAllocator()
    : _allocation_count(0), _allocation_bytes(0)
{
}

MemoryAllocator::~MemoryAllocator() throw()
{
}

/*void* MemoryAllocator::allocate_array(size_t bytes)
{
    // need to allocate a little extra for systems
    // that store the size of the array in memory
    return allocate(bytes + ARRAY_OFFSET);
}

void MemoryAllocator::release_array(void* ptr)
{
    // for systems that store the size of the array
    // in memory, we need to back up over that to release
    release(reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr) - ARRAY_OFFSET));
}*/

void* MemoryAllocator::allocate(size_t bytes, size_t alignment)
{
    // Game Engine Architecture 5.2.1.3
    assert(alignment > 1);

    // total memory (worst case)
    size_t expanded = bytes + alignment;

    // allocate unaligned block
    size_t address = reinterpret_cast<size_t>(allocate(expanded));

    // calculate adjustment by masking off the lower bits
    // of the address, to determine how "misaligned" it is
    size_t mask = alignment - 1;
    size_t misalignment = address & mask;

    // adjustment will always be at least 1 byte
    size_t adjustment = alignment - misalignment;

    // adjusted address
    size_t aligned = address + adjustment;

    // store the adjustment in the four bytes immediately
    // preceding the adjusted address that we're returning
    unsigned char* adjusted = reinterpret_cast<unsigned char*>(aligned - 4);
    *adjusted = adjustment;

    return reinterpret_cast<void*>(aligned);
}

/*void* MemoryAllocator::allocate_array(size_t bytes, size_t alignment)
{
    // need to allocate a little extra for systems
    // that store the size of the array in memory
    return allocate(bytes + ARRAY_OFFSET, alignment);
}*/

void MemoryAllocator::release(void* ptr, size_t alignement)
{
    // Game Engine Architecture 5.2.1.3
    size_t aligned = reinterpret_cast<size_t>(ptr);
    uint8_t* adjusted = reinterpret_cast<uint8_t*>(aligned - 4);
    size_t adjustment = static_cast<size_t>(*adjusted);
    size_t address = aligned - adjustment;
    release(reinterpret_cast<void*>(address));
}

/*void MemoryAllocator::release_array(void* ptr, size_t alignment)
{
    // for systems that store the size of the array
    // in memory, we need to back up over that to release
    release(reinterpret_cast<void*>(reinterpret_cast<size_t>(ptr) - ARRAY_OFFSET), alignment);
}*/

}
