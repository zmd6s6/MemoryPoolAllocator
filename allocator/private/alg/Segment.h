#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <alg/Node.h>


constexpr auto DEFAULT_PAGE_SIZE = 4096;

struct SegmentContext {
    size_t blockCount;          
    std::vector<void *> pages;
    Node *freeList;
};
#endif //SEGMENT_H
