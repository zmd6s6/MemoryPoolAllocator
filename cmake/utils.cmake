macro(add_standard_module target is_shared)
    if (${target} STREQUAL "")
        message(FATAL_ERROR "未传入Target名称")
    endif ()
    if (NOT ${is_shared})
        add_library(${target})
    else ()
        add_library(${target} SHARED)
    endif ()
    file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
    file(GLOB_RECURSE srcs_p CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/private/*.h")
    target_include_directories(${target} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)
    target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")
    target_sources(${target} PRIVATE ${srcs} ${srcs_p})
endmacro()