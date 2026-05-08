#ifndef JZMEMORYPOOLALLOCATOR_FSMEMORYPOOL_H
#define JZMEMORYPOOLALLOCATOR_FSMEMORYPOOL_H

#include <memory>
#include <allocator/EXPORT.h>

class FsMemoryPoolPrivate;

namespace jz
{

    class JZ_ALLOCATOR_API FsMemoryPool
    {
        std::unique_ptr<FsMemoryPoolPrivate> d;
        friend class FsMemoryPoolPrivate;

    public:
        explicit FsMemoryPool(std::size_t size);
        ~FsMemoryPool();
        FsMemoryPool(FsMemoryPool&&) noexcept;
        FsMemoryPool& operator=(FsMemoryPool&&) noexcept;
        void* allocate();
        void deallocate(void* p);
        [[nodiscard]] std::size_t blockSize() const;
    };
} // jz

#endif //JZMEMORYPOOLALLOCATOR_FSMEMORYPOOL_H
