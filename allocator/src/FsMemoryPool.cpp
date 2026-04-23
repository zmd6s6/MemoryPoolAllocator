#include <allocator/FsMemoryPool.h>
#include <allocator/FsMemoryPoolPrivate.h>

namespace jz
{
    FsMemoryPool::FsMemoryPool(std::size_t size, std::size_t blockSizePerPage) : d(
        std::make_unique<FsMemoryPoolPrivate>(this))
    {
        d->blockCountPerPage = blockSizePerPage;
        d->blockSize = size;
    }

    FsMemoryPool::~FsMemoryPool()
    {
    }

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

    std::size_t FsMemoryPool::blockCountPerPage() const { return d->blockCountPerPage; }
} // jz
