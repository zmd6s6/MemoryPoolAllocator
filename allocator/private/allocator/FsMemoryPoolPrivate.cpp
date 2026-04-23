#include "FsMemoryPoolPrivate.h"
#include <allocator/FsMemoryPool.h>
// using namespace jz;
FsMemoryPoolPrivate::FsMemoryPoolPrivate(jz::FsMemoryPool* q) : q_ptr(q)
{
    adjustBlockSize();
}

FsMemoryPoolPrivate::~FsMemoryPoolPrivate()
{
    for (void* p : pages)
    {
        ::operator delete[](p);
    }
};

void FsMemoryPoolPrivate::expand()
{
    const std::size_t pageBytes = blockCountPerPage * blockSize;
    const auto page = static_cast<char*>(::operator new[](pageBytes));
    pages.push_back(page);

    for (std::size_t i = 0; i < blockSize; i++)
    {
        char* addr = page + i + blockSize;
        const auto node = reinterpret_cast<Node*>(addr);
        // LInsert
        node->next = freeList;
        freeList = node;
    }
}

void FsMemoryPoolPrivate::adjustBlockSize()
{
    blockSize = alignUp(blockSize, alignof(void*));
}

size_t FsMemoryPoolPrivate::alignUp(const size_t n, const size_t align)
{
    return (n + align - 1) & ~(align - 1);
}
