#ifndef DMMEMORYPOOLPRIVATE_H
#define DMMEMORYPOOLPRIVATE_H

#include <allocator/DmMemoryPool.h>

class DmMemoryPoolPrivate {
public:
    DmMemoryPoolPrivate(jz::DmMemoryPool* q);
    ~DmMemoryPoolPrivate();

    jz::DmMemoryPool* q_ptr;
};



#endif //DMMEMORYPOOLPRIVATE_H
