#include <allocator/DmMemoryPool.h>
#include <allocator/DmMemoryPoolPrivate.h>

namespace jz {
    DmMemoryPool::DmMemoryPool()
        : d(std::make_unique<DmMemoryPoolPrivate>(this)) {
    }

    DmMemoryPool::~DmMemoryPool() = default;

    void* DmMemoryPool::allocate(size_t size) {
        if (size == 0) return nullptr;

        const size_t alignedSize = DmMemoryPoolPrivate::alignUp(size, alignof(void*));

        while (true) {
            auto it = d->levelSegment.find(alignedSize);
            if (it != d->levelSegment.end()) {
                for (auto& seg : it->second) {
                    if (seg.freeList) {
                        auto* node = seg.freeList;
                        seg.freeList = node->next;
                        seg.freeCount--;
                        d->ptrMap[node] = {seg.page, alignedSize};
                        return node;
                    }
                }
            }
            d->expandLevel(alignedSize);
        }
    }

    void DmMemoryPool::deallocate(void* p) {
        if (!p) return;

        auto it = d->ptrMap.find(p);
        if (it == d->ptrMap.end()) return;

        auto [page, blockSize] = it->second;
        d->ptrMap.erase(it);

        auto& segments = d->levelSegment[blockSize];
        for (auto& seg : segments) {
            if (seg.page == page) {
                auto* node = static_cast<Node*>(p);
                node->next = seg.freeList;
                seg.freeList = node;
                seg.freeCount++;
                break;
            }
        }
    }
} // jz
