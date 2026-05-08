# MemoryPoolAllocator

固定块大小内存池，支持自动扩容和自动对齐。

## 特性

- **固定块大小** —— 每个 block 大小在构造时指定，分配时返回固定大小的内存块
- **自动扩容** —— 按页为单位自动申请新内存，页内所有 block 被拆解加入空闲链表
- **自动对齐** —— block 大小自动向上对齐到指针边界（`alignof(void*)`）
- **内存复用** —— `deallocate` 归还的 block 进入空闲链表，后续 `allocate` 优先复用
- **移动语义** —— 支持移动构造和移动赋值
- **Grow-only** —— 池是 **只增不缩** 的，一旦分配的页不会释放回系统直到池析构。

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
| `FsMemoryPool::allocate()` | 分配一个 block，自动扩容，不会失败 |
| `FsMemoryPool::deallocate(p)` | 归还 block；传入 `nullptr` 安全 |
| `FsMemoryPool::blockSize()` | 返回对齐后的 block 大小 |
| `DmMemoryPool()` | 动态分级内存池，自动管理不同大小的 block |
| `DmMemoryPool::allocate(size)` | 分配指定大小的 block，自动创建对应 size 的 level |
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

## 设计说明

### Grow-only 策略

两个内存池均为 **只增不缩** 设计。分配出去的 block 归还后进入空闲链表复用，但已申请的页（page）不会释放回系统，直到池析构。

**原因：**
- 分配/释放均为 O(1)，无碎片
- 适应高频分配场景（如游戏帧循环、消息处理）
- `freeCount` 计数可用于判断整页空闲，如需可在此基础上实现页回收
