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
    explicit FsMemoryPool(std::size_t blockSize,
                          std::size_t blockCountPerPage = 1024);
    ~FsMemoryPool();

    FsMemoryPool(FsMemoryPool&&) noexcept;
    FsMemoryPool& operator=(FsMemoryPool&&) noexcept;

    void* allocate();
    void deallocate(void* p);

    std::size_t blockSize() const;
    std::size_t blockCountPerPage() const;
};

}
```

| 方法 | 说明 |
|---|---|
| `FsMemoryPool(size, blockCountPerPage)` | 构造内存池，`blockSize` 为每个 block 字节数，`blockCountPerPage` 为每次扩容的 block 数量 |
| `allocate()` | 分配一个 block，返回非空指针；自动扩容 |
| `deallocate(p)` | 归还 block 到空闲链表；传入 `nullptr` 安全 |
| `blockSize()` | 返回对齐后的 block 大小 |
| `blockCountPerPage()` | 返回每次扩容的 block 数量 |

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
- 单 block 每页
- 压力测试

## 项目结构

```
MemoryPoolAllocator/
├── allocator/
│   ├── include/allocator/FsMemoryPool.h    # 公开头文件
│   ├── src/FsMemoryPool.cpp                # 公开接口实现
│   └── private/allocator/                  # 私有实现（d-pointer）
│       ├── FsMemoryPoolPrivate.h
│       └── FsMemoryPoolPrivate.cpp
├── test/
│   ├── CMakeLists.txt
│   └── test_FsMemoryPool.cpp               # 单元测试
├── third_party/googletest-1.14.0/           # Google Test 源码
├── cmake/utils.cmake                        # CMake 辅助宏
└── CMakeLists.txt
```

## TODO

- [ ] **DmMemoryPool** —— 动态分级内存池，支持不同 block 大小的分级管理
