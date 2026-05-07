#ifndef DMMEMORYPOOLPRIVATE_H
#define DMMEMORYPOOLPRIVATE_H

#include <map>
#include <vector>
#include <allocator/DmMemoryPool.h>
#include <alg/Segment.h>


class DmMemoryPoolPrivate {
public:
    explicit DmMemoryPoolPrivate(jz::DmMemoryPool* q);
    ~DmMemoryPoolPrivate();

    jz::DmMemoryPool* q_ptr;
    // log2n定位
    std::vector<SegmentContext> segment;
};



#endif //DMMEMORYPOOLPRIVATE_H
