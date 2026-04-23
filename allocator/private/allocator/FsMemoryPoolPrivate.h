#ifndef JZMEMORYPOOLALLOCATOR_FSMEMORYPOOLPRIVATE_H
#define JZMEMORYPOOLALLOCATOR_FSMEMORYPOOLPRIVATE_H

#include <vector>
namespace jz
{
    class FsMemoryPool;
}

struct  Node
{
    Node* next;
};

class FsMemoryPoolPrivate
{
public:
    explicit FsMemoryPoolPrivate(jz::FsMemoryPool* q);
    ~FsMemoryPoolPrivate();
    void expand();
    void adjustBlockSize();
    static size_t alignUp(size_t n,size_t align);

    jz::FsMemoryPool* q_ptr;
    friend class FSMemoryPool;

    size_t              blockSize{};
    size_t              blockCountPerPage{};
    std::vector<void*>  pages;
    Node*               freeList{};
};


#endif //JZMEMORYPOOLALLOCATOR_FSMEMORYPOOLPRIVATE_H
