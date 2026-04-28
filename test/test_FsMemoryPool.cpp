#include <gtest/gtest.h>
#include <allocator/FsMemoryPool.h>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

class FsMemoryPoolTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FsMemoryPoolTest, Construction)
{
    jz::FsMemoryPool pool(32);
    EXPECT_EQ(pool.blockSize(), 32);
    EXPECT_EQ(pool.blockCountPerPage(), 1024);
}

TEST_F(FsMemoryPoolTest, CustomBlockCountPerPage)
{
    jz::FsMemoryPool pool(64, 512);
    EXPECT_EQ(pool.blockSize(), 64);
    EXPECT_EQ(pool.blockCountPerPage(), 512);
}

TEST_F(FsMemoryPoolTest, BlockSizeAligned)
{
    jz::FsMemoryPool pool(1);
    EXPECT_GE(pool.blockSize(), sizeof(void*));
    EXPECT_EQ(pool.blockSize() % alignof(void*), 0);
}

TEST_F(FsMemoryPoolTest, AllocateReturnsNonNull)
{
    jz::FsMemoryPool pool(64, 10);
    void* p = pool.allocate();
    EXPECT_NE(p, nullptr);
}

TEST_F(FsMemoryPoolTest, AllocateReturnsAlignedBlock)
{
    jz::FsMemoryPool pool(32, 10);
    void* p = pool.allocate();
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % alignof(void*), 0);
}

TEST_F(FsMemoryPoolTest, AllocateMultipleBlocks)
{
    constexpr std::size_t count = 100;
    jz::FsMemoryPool pool(32, count);
    std::vector<void*> ptrs;
    for (std::size_t i = 0; i < count; ++i)
    {
        void* p = pool.allocate();
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    std::set<void*> unique(ptrs.begin(), ptrs.end());
    EXPECT_EQ(unique.size(), count);
}

TEST_F(FsMemoryPoolTest, AutoExpand)
{
    jz::FsMemoryPool pool(32, 5);
    std::vector<void*> ptrs;
    for (std::size_t i = 0; i < 20; ++i)
    {
        void* p = pool.allocate();
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    std::set<void*> unique(ptrs.begin(), ptrs.end());
    EXPECT_EQ(unique.size(), 20);
}

TEST_F(FsMemoryPoolTest, AllocateAndDeallocateReuse)
{
    jz::FsMemoryPool pool(32, 5);
    void* p1 = pool.allocate();
    ASSERT_NE(p1, nullptr);
    pool.deallocate(p1);
    void* p2 = pool.allocate();
    EXPECT_EQ(p1, p2);
}

TEST_F(FsMemoryPoolTest, AllocateAllAndReuseAll)
{
    constexpr std::size_t count = 10;
    jz::FsMemoryPool pool(64, count);
    std::vector<void*> ptrs;
    for (std::size_t i = 0; i < count; ++i)
        ptrs.push_back(pool.allocate());
    for (void* p : ptrs)
        pool.deallocate(p);
    for (std::size_t i = 0; i < count; ++i)
    {
        void* p = pool.allocate();
        EXPECT_NE(p, nullptr);
    }
}

TEST_F(FsMemoryPoolTest, DeallocateNullIsSafe)
{
    jz::FsMemoryPool pool(32);
    pool.deallocate(nullptr);
    void* p = pool.allocate();
    EXPECT_NE(p, nullptr);
}

TEST_F(FsMemoryPoolTest, StressTest)
{
    constexpr std::size_t iterations = 1000;
    constexpr std::size_t blockSize = 32;
    jz::FsMemoryPool pool(blockSize, 64);
    for (std::size_t i = 0; i < iterations; ++i)
    {
        void* p = pool.allocate();
        ASSERT_NE(p, nullptr);
        std::memset(p, 0xAB, blockSize);
        pool.deallocate(p);
    }
}

TEST_F(FsMemoryPoolTest, BlocksDoNotOverlap)
{
    constexpr std::size_t count = 50;
    constexpr std::size_t blockSize = 16;
    jz::FsMemoryPool pool(blockSize, count);
    std::vector<void*> ptrs;
    for (std::size_t i = 0; i < count; ++i)
        ptrs.push_back(pool.allocate());
    for (std::size_t i = 0; i < ptrs.size(); ++i)
    {
        for (std::size_t j = i + 1; j < ptrs.size(); ++j)
        {
            auto a = reinterpret_cast<std::uintptr_t>(ptrs[i]);
            auto b = reinterpret_cast<std::uintptr_t>(ptrs[j]);
            bool overlap = (a < b && a + blockSize > b) || (b < a && b + blockSize > a);
            EXPECT_FALSE(overlap);
        }
    }
}

TEST_F(FsMemoryPoolTest, WriteToAllocatedBlocks)
{
    constexpr std::size_t count = 100;
    constexpr std::size_t blockSize = 64;
    jz::FsMemoryPool pool(blockSize, 16);
    std::vector<void*> ptrs;
    for (std::size_t i = 0; i < count; ++i)
    {
        void* p = pool.allocate();
        ASSERT_NE(p, nullptr);
        std::memset(p, static_cast<int>(i & 0xFF), blockSize);
        ptrs.push_back(p);
    }
    for (std::size_t i = 0; i < count; ++i)
    {
        auto* data = static_cast<unsigned char*>(ptrs[i]);
        for (std::size_t j = 0; j < blockSize; ++j)
            EXPECT_EQ(data[j], static_cast<unsigned char>(i & 0xFF));
    }
}

TEST_F(FsMemoryPoolTest, MoveConstruct)
{
    jz::FsMemoryPool pool(32, 10);
    void* p1 = pool.allocate();
    ASSERT_NE(p1, nullptr);
    jz::FsMemoryPool moved(std::move(pool));
    void* p2 = moved.allocate();
    EXPECT_NE(p2, nullptr);
    EXPECT_NE(p1, p2);
}

TEST_F(FsMemoryPoolTest, MoveAssign)
{
    jz::FsMemoryPool pool(32, 10);
    void* p1 = pool.allocate();
    ASSERT_NE(p1, nullptr);
    jz::FsMemoryPool other(64, 5);
    other = std::move(pool);
    void* p2 = other.allocate();
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(other.blockSize(), 32);
}

TEST_F(FsMemoryPoolTest, LargeBlockSize)
{
    constexpr std::size_t largeSize = 4096;
    jz::FsMemoryPool pool(largeSize, 4);
    EXPECT_EQ(pool.blockSize(), largeSize);
    void* p = pool.allocate();
    ASSERT_NE(p, nullptr);
    std::memset(p, 0xFF, largeSize);
}

TEST_F(FsMemoryPoolTest, SingleBlockPerPage)
{
    jz::FsMemoryPool pool(64, 1);
    void* p1 = pool.allocate();
    ASSERT_NE(p1, nullptr);
    void* p2 = pool.allocate();
    ASSERT_NE(p2, nullptr);
    EXPECT_NE(p1, p2);
    pool.deallocate(p1);
    pool.deallocate(p2);
}
