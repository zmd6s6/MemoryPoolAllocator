#include "DmMemoryPoolPrivate.h"

DmMemoryPoolPrivate::DmMemoryPoolPrivate(jz::DmMemoryPool* q) : q_ptr(q) {
}

DmMemoryPoolPrivate::~DmMemoryPoolPrivate() {
    for (auto& [size, segments] : levelSegment) {
        for (auto& seg : segments) {
            ::operator delete[](static_cast<char*>(seg.page));
        }
    }
}

void DmMemoryPoolPrivate::expandLevel(size_t blockSize) {
    const size_t pageBytes = blockSize * kDefaultBlockCount;
    auto* page = static_cast<char*>(::operator new[](pageBytes));

    SegmentContext seg;
    seg.blockSize = blockSize;
    seg.blockCount = kDefaultBlockCount;
    seg.page = page;
    seg.freeCount = kDefaultBlockCount;

    Node* head = nullptr;
    for (size_t i = 0; i < kDefaultBlockCount; i++) {
        auto* node = reinterpret_cast<Node*>(page + i * blockSize);
        node->next = head;
        head = node;
    }
    seg.freeList = head;

    levelSegment[blockSize].push_back(std::move(seg));
}

size_t DmMemoryPoolPrivate::alignUp(size_t n, size_t align) {
    return (n + align - 1) & ~(align - 1);
}
