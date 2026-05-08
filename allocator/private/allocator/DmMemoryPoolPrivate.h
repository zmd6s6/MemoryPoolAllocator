#ifndef DMMEMORYPOOLPRIVATE_H
#define DMMEMORYPOOLPRIVATE_H

#include <map>
#include <vector>
#include <unordered_map>
#include <cstddef>
#include <allocator/DmMemoryPool.h>
#include <alg/Segment.h>
#include <alg/Node.h>

struct BlockInfo {
    void* page;
    size_t blockSize;
};

class DmMemoryPoolPrivate {
public:
    explicit DmMemoryPoolPrivate(jz::DmMemoryPool* q);
    ~DmMemoryPoolPrivate();

    static constexpr size_t kDefaultBlockCount = 64;
    void expandLevel(size_t blockSize);
    static size_t alignUp(size_t n, size_t align);

    jz::DmMemoryPool* q_ptr;
    std::map<size_t, std::vector<SegmentContext>> levelSegment;
    std::unordered_map<void*, BlockInfo> ptrMap;
};

#endif //DMMEMORYPOOLPRIVATE_H
