#include <allocator/FsMemoryPool.h>
#include <allocator/FsMemoryPoolPrivate.h>

namespace jz
{
    FsMemoryPool::FsMemoryPool(std::size_t size) : d(
        std::make_unique<FsMemoryPoolPrivate>(this))
    {
        d->blockCountPerPage = FsMemoryPoolPrivate::kDefaultBlockCount;
        d->blockSize = size;
        d->adjustBlockSize();
    }

    FsMemoryPool::~FsMemoryPool() = default;
    FsMemoryPool::FsMemoryPool(FsMemoryPool&&) noexcept = default;
    FsMemoryPool& FsMemoryPool::operator=(FsMemoryPool&&) noexcept = default;

    void* FsMemoryPool::allocate()
    {
        if (!d->freeList)
            d->expand();
        Node* head = d->freeList;
        d->freeList = head->next;
        return head;
    }

    void FsMemoryPool::deallocate(void* p)
    {
        if (!p)
            return;
        Node* node = static_cast<Node*>(p);
        node->next = d->freeList;
        d->freeList = node;
    }

    std::size_t FsMemoryPool::blockSize() const { return d->blockSize; }
} // jz
