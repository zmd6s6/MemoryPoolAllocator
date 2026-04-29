#ifndef EXPORT_H
#define EXPORT_H

#ifdef jzAllocator_EXPORTS
#define JZ_ALLOCATOR_API __declspec(dllexport)
#else
#define JZ_ALLOCATOR_API __declspec(dllimport)
#endif


#endif //EXPORT_H
