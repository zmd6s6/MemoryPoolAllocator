#ifndef DMMEMORYPOOL_H
#define DMMEMORYPOOL_H
#include <memory>
#include <allocator/EXPORT.h>

class DmMemoryPoolPrivate;

namespace jz {
    class JZ_ALLOCATOR_API DmMemoryPool {
        std::unique_ptr<DmMemoryPoolPrivate> d;
    public:
        explicit DmMemoryPool();
        ~DmMemoryPool();
        void* allocate(size_t size);
        void deallocate(void *p);
    };
} // jz

#endif //DMMEMORYPOOL_H
