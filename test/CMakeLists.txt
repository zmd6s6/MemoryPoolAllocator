cmake_minimum_required(VERSION 3.16)
project(jzAllocatorTest)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest-1.14.0 googletest)

add_executable(test_allocator test_FsMemoryPool.cpp test_DmMemoryPool.cpp)
target_link_libraries(test_allocator PRIVATE jzAllocator GTest::gtest_main)
target_include_directories(test_allocator PRIVATE ${CMAKE_SOURCE_DIR}/allocator/include)
add_custom_command(TARGET test_allocator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:jzAllocator> $<TARGET_FILE_DIR:test_allocator>
)
include(GoogleTest)
gtest_discover_tests(test_allocator)
