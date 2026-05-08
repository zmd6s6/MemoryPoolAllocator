# MemoryPoolAllocator

固定块大小内存池，支持自动扩容和自动对齐。

## 特性

- **固定块大小** —— 每个 block 大小在构造时指定，分配时返回固定大小的内存块
- **自动扩容** —— 按页（Page）为单位自动申请新内存，页内所有 block 被拆解加入空闲链表
- **自动对齐** —— block 大小自动向上对齐到指针边界（`alignof(void*)`）
- **内存复用** —— `deallocate` 归还的 block 会进入空闲链表，后续 `allocate` 优先复用
- **移动语义** —— 支持移动构造和移动赋值

## API

```cpp
namespace jz {

class FsMemoryPool {
public:
    explicit FsMemoryPool(std::size_t blockSize);
    ~FsMemoryPool();

    FsMemoryPool(FsMemoryPool&&) noexcept;
    FsMemoryPool& operator=(FsMemoryPool&&) noexcept;

    void* allocate();
    void deallocate(void* p);

    std::size_t blockSize() const;
};

class DmMemoryPool {
public:
    DmMemoryPool();
    ~DmMemoryPool();

    void* allocate(std::size_t size);
    void deallocate(void* p);
};

}
```

| 方法 | 说明 |
|---|---|
| `FsMemoryPool(size)` | 固定块大小内存池，`blockSize` 为每个 block 字节数 |
| `FsMemoryPool::allocate()` | 分配一个 block，返回非空指针；自动扩容 |
| `FsMemoryPool::deallocate(p)` | 归还 block 到空闲链表；传入 `nullptr` 安全 |
| `FsMemoryPool::blockSize()` | 返回对齐后的 block 大小 |
| `DmMemoryPool()` | 动态分级内存池，自动管理不同大小的 block |
| `DmMemoryPool::allocate(size)` | 分配指定大小的 block |
| `DmMemoryPool::deallocate(p)` | 归还 block |

## 构建

### 要求
- C++17 编译器（MSVC / GCC / Clang）
- CMake 3.16+

### 步骤

```bash
# 配置
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

# 构建库
cmake --build build --target jzAllocator

# 构建并运行测试
cmake --build build
ctest --test-dir build -C Debug
```

## 测试

使用 Google Test（已捆绑在 `third_party/googletest-1.14.0`），覆盖以下场景：

- 构造与参数检查
- 对齐保证
- 单次/多次分配
- 自动扩容
- 分配-归还-复用
- `deallocate(nullptr)` 安全性
- 内存不重叠
- 数据完整性
- 移动语义
- 大块分配
- 压力测试

**DmMemoryPool：** 12 个测试用例覆盖多级分配、内存复用、数据完整性、压力测试

## 项目结构

```
MemoryPoolAllocator/
├── allocator/
│   ├── include/allocator/
│   │   ├── FsMemoryPool.h                  # FsMemoryPool 公开头文件
│   │   ├── DmMemoryPool.h                  # DmMemoryPool 公开头文件
│   │   └── EXPORT.h                        # DLL 导出宏
│   ├── src/
│   │   ├── FsMemoryPool.cpp
│   │   └── DmMemoryPool.cpp
│   └── private/
│       ├── allocator/                      # 私有实现（d-pointer）
│       │   ├── FsMemoryPoolPrivate.h/cpp
│       │   └── DmMemoryPoolPrivate.h/cpp
│       └── alg/                            # 内部数据结构
│           ├── Node.h
│           └── Segment.h
├── test/
│   ├── CMakeLists.txt
│   ├── test_FsMemoryPool.cpp
│   └── test_DmMemoryPool.cpp
├── third_party/googletest-1.14.0/           # Google Test 源码
├── cmake/utils.cmake                        # CMake 辅助宏
└── CMakeLists.txt
```

## TODO

- [ ] **DmMemoryPool** —— 动态分级内存池，支持不同 block 大小的分级管理
