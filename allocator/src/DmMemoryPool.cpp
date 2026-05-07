#include <cmath>
#include <allocator/DmMemoryPool.h>
#include <allocator/DmMemoryPoolPrivate.h>


namespace jz {
    DmMemoryPool::DmMemoryPool()
        : d(std::make_unique<DmMemoryPoolPrivate>(this)) {
    }

    DmMemoryPool::~DmMemoryPool() {
    }

    void *DmMemoryPool::allocate(size_t size) {
        if (size == 0)
            return nullptr;
        if (!d->segment.at(std::log2(size)).freeList) {

        }
        return nullptr;
    }

    void DmMemoryPool::deallocate(void *p) {
    }
} // jz
