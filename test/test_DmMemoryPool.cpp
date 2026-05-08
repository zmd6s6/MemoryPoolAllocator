#include <gtest/gtest.h>
#include <allocator/DmMemoryPool.h>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

class DmMemoryPoolTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DmMemoryPoolTest, AllocateSingleBlock)
{
    jz::DmMemoryPool pool;
    void* p = pool.allocate(32);
    EXPECT_NE(p, nullptr);
}

TEST_F(DmMemoryPoolTest, AllocateReturnsAlignedBlock)
{
    jz::DmMemoryPool pool;
    void* p = pool.allocate(1);
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) % alignof(void*), 0);
}

TEST_F(DmMemoryPoolTest, AllocateZeroReturnsNull)
{
    jz::DmMemoryPool pool;
    void* p = pool.allocate(0);
    EXPECT_EQ(p, nullptr);
}

TEST_F(DmMemoryPoolTest, DeallocateNullIsSafe)
{
    jz::DmMemoryPool pool;
    pool.deallocate(nullptr);
}

TEST_F(DmMemoryPoolTest, MultipleSizes)
{
    jz::DmMemoryPool pool;
    void* p8 = pool.allocate(8);
    void* p16 = pool.allocate(16);
    void* p32 = pool.allocate(32);
    void* p64 = pool.allocate(64);
    EXPECT_NE(p8, nullptr);
    EXPECT_NE(p16, nullptr);
    EXPECT_NE(p32, nullptr);
    EXPECT_NE(p64, nullptr);
    pool.deallocate(p8);
    pool.deallocate(p16);
    pool.deallocate(p32);
    pool.deallocate(p64);
}

TEST_F(DmMemoryPoolTest, ReuseAfterDeallocate)
{
    jz::DmMemoryPool pool;
    void* p1 = pool.allocate(32);
    ASSERT_NE(p1, nullptr);
    pool.deallocate(p1);
    void* p2 = pool.allocate(32);
    EXPECT_EQ(p1, p2);
}

TEST_F(DmMemoryPoolTest, AllocateManySameSize)
{
    jz::DmMemoryPool pool;
    constexpr int count = 200;
    std::vector<void*> ptrs;
    for (int i = 0; i < count; ++i)
    {
        void* p = pool.allocate(16);
        ASSERT_NE(p, nullptr);
        ptrs.push_back(p);
    }
    std::set<void*> unique(ptrs.begin(), ptrs.end());
    EXPECT_EQ(unique.size(), static_cast<size_t>(count));
    for (void* p : ptrs)
        pool.deallocate(p);
}

TEST_F(DmMemoryPoolTest, WriteAndRead)
{
    jz::DmMemoryPool pool;
    void* p = pool.allocate(128);
    ASSERT_NE(p, nullptr);
    std::memset(p, 0x5A, 128);
    auto* data = static_cast<unsigned char*>(p);
    for (int i = 0; i < 128; ++i)
        EXPECT_EQ(data[i], 0x5A);
    pool.deallocate(p);
}

TEST_F(DmMemoryPoolTest, MultipleSizesDontInterfere)
{
    jz::DmMemoryPool pool;
    void* p32 = pool.allocate(32);
    void* p64 = pool.allocate(64);
    ASSERT_NE(p32, nullptr);
    ASSERT_NE(p64, nullptr);
    std::memset(p32, 0xAB, 32);
    std::memset(p64, 0xCD, 64);
    EXPECT_EQ(static_cast<unsigned char*>(p32)[0], 0xAB);
    EXPECT_EQ(static_cast<unsigned char*>(p64)[0], 0xCD);
    EXPECT_NE(p32, p64);
    pool.deallocate(p32);
    pool.deallocate(p64);
}

TEST_F(DmMemoryPoolTest, BlocksDoNotOverlap)
{
    jz::DmMemoryPool pool;
    constexpr int count = 50;
    constexpr size_t blockSize = 16;
    std::vector<void*> ptrs;
    for (int i = 0; i < count; ++i)
        ptrs.push_back(pool.allocate(blockSize));
    for (size_t i = 0; i < ptrs.size(); ++i)
    {
        for (size_t j = i + 1; j < ptrs.size(); ++j)
        {
            auto a = reinterpret_cast<std::uintptr_t>(ptrs[i]);
            auto b = reinterpret_cast<std::uintptr_t>(ptrs[j]);
            bool overlap = (a < b && a + blockSize > b) || (b < a && b + blockSize > a);
            EXPECT_FALSE(overlap);
        }
    }
    for (void* p : ptrs)
        pool.deallocate(p);
}

TEST_F(DmMemoryPoolTest, StressTest)
{
    jz::DmMemoryPool pool;
    constexpr int iterations = 500;
    for (int i = 0; i < iterations; ++i)
    {
        size_t size = (i % 7 + 1) * 8;
        void* p = pool.allocate(size);
        ASSERT_NE(p, nullptr);
        std::memset(p, static_cast<int>(i & 0xFF), size);
        pool.deallocate(p);
    }
}

TEST_F(DmMemoryPoolTest, InterleavedSizes)
{
    jz::DmMemoryPool pool;
    std::vector<std::pair<void*, size_t>> blocks;
    for (int i = 0; i < 100; ++i)
    {
        size_t size = (1 << (i % 6 + 3));
        void* p = pool.allocate(size);
        ASSERT_NE(p, nullptr);
        std::memset(p, static_cast<int>(i & 0xFF), size);
        blocks.emplace_back(p, size);
    }
    // Verify data integrity after all allocations
    for (size_t k = 0; k < blocks.size(); ++k)
    {
        auto& [p, size] = blocks[k];
        auto* data = static_cast<unsigned char*>(p);
        unsigned char expected = static_cast<unsigned char>(k & 0xFF);
        for (size_t j = 0; j < size; ++j)
            EXPECT_EQ(data[j], expected);
        pool.deallocate(p);
    }
}
