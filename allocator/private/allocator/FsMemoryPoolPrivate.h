#ifndef JZMEMORYPOOLALLOCATOR_FSMEMORYPOOLPRIVATE_H
#define JZMEMORYPOOLALLOCATOR_FSMEMORYPOOLPRIVATE_H

#include <vector>
#include <alg/Node.h>

namespace jz
{
    class FsMemoryPool;
}

class FsMemoryPoolPrivate
{
public:
    explicit FsMemoryPoolPrivate(jz::FsMemoryPool* q);
    ~FsMemoryPoolPrivate();
    void expand();
    void adjustBlockSize();
    static size_t alignUp(size_t n,size_t align);

    static constexpr size_t kDefaultBlockCount = 64;

    jz::FsMemoryPool* q_ptr;
    friend class FSMemoryPool;

    size_t              blockSize{};
    size_t              blockCountPerPage{kDefaultBlockCount};
    std::vector<void*>  pages;
    Node*               freeList{};
};


#endif //JZMEMORYPOOLALLOCATOR_FSMEMORYPOOLPRIVATE_H
