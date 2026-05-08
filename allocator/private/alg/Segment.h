#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstddef>
#include <alg/Node.h>

struct SegmentContext {
    size_t blockSize;
    size_t blockCount;
    void* page;
    Node* freeList;
    size_t freeCount;
};
#endif //SEGMENT_H
