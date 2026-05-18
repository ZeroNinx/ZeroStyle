cmake_minimum_required(VERSION 3.20)
project(ZeroStyle VERSION 1.0.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# ==============================================================================
# 选项
# ==============================================================================

option(ZERO_USE_CXX23    "使用 C++23（可用时启用 std::expected 后端）" OFF)
option(ZERO_BUILD_TESTS  "编译冒烟测试"                          OFF)
option(ZERO_BUILD_EXAMPLES "编译示例程序"                         OFF)
option(ZERO_ENABLE_WARNINGS "为 ZeroStyle 自带测试和示例启用警告"   ON)
option(ZERO_WARNINGS_AS_ERRORS "将 ZeroStyle 自带目标的警告视为错误" OFF)
option(ZERO_ENABLE_THREAD_SAFETY_ANALYSIS
    "为 ZeroStyle 消费者启用 Clang 线程安全分析"
    OFF)

# Single-config generators (Ninja, Make): honour CMAKE_BUILD_TYPE.
# Multi-config generators (Visual Studio, Xcode): use --config at build time.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR
   AND NOT CMAKE_CONFIGURATION_TYPES
   AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# MSVC: treat source files as UTF-8, suppress C4819
if(MSVC)
    add_compile_options(/utf-8)
endif()

function(zero_apply_project_warnings TargetName)
    if(NOT ZERO_ENABLE_WARNINGS)
        return()
    endif()

    if(MSVC)
        target_compile_options(${TargetName} PRIVATE /W4)

        if(ZERO_WARNINGS_AS_ERRORS)
            target_compile_options(${TargetName} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${TargetName} PRIVATE -Wall -Wextra -Wpedantic)

        if(ZERO_WARNINGS_AS_ERRORS)
            target_compile_options(${TargetName} PRIVATE -Werror)
        endif()
    endif()
endfunction()

# ==============================================================================
# Header-only INTERFACE 目标
# ==============================================================================

add_library(ZeroStyle INTERFACE)
add_library(Zero::ZeroStyle ALIAS ZeroStyle)

target_include_directories(ZeroStyle INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(ZERO_USE_CXX23)
    target_compile_features(ZeroStyle INTERFACE cxx_std_23)
else()
    target_compile_features(ZeroStyle INTERFACE cxx_std_20)
endif()

# Clang 线程安全分析。默认关闭，避免意外影响消费者。
if(ZERO_ENABLE_THREAD_SAFETY_ANALYSIS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(ZeroStyle INTERFACE -Wthread-safety)
endif()

# ==============================================================================
# 冒烟测试（-DZERO_BUILD_TESTS=ON）
# ==============================================================================

if(ZERO_BUILD_TESTS)
    enable_testing()

    function(zero_add_smoke_test TestName SourceFile)
        add_executable(ZeroStyle_${TestName} ${SourceFile})
        target_link_libraries(ZeroStyle_${TestName} PRIVATE Zero::ZeroStyle)
        zero_apply_project_warnings(ZeroStyle_${TestName})
        add_test(NAME ZeroStyle.${TestName} COMMAND ZeroStyle_${TestName})
    endfunction()

    zero_add_smoke_test(MacrosSmokeTest      test/MacrosSmokeTest.cpp)
    zero_add_smoke_test(ResultSmokeTest      test/ResultSmokeTest.cpp)
    zero_add_smoke_test(ScopeFlagsSmokeTest  test/ScopeFlagsSmokeTest.cpp)
    zero_add_smoke_test(ShortMacrosSmokeTest test/ShortMacrosSmokeTest.cpp)
    zero_add_smoke_test(StringPathSmokeTest  test/StringPathSmokeTest.cpp)
    zero_add_smoke_test(TypesConceptsSmokeTest test/TypesConceptsSmokeTest.cpp)
endif()

# ==============================================================================
# 示例（-DZERO_BUILD_EXAMPLES=ON）
# ==============================================================================

if(ZERO_BUILD_EXAMPLES)
    add_subdirectory(examples/basic-console)
endif()

# ==============================================================================
# Install / package export
# ==============================================================================

install(TARGETS ZeroStyle
    EXPORT ZeroStyleTargets
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ZeroStyleTargets
    FILE ZeroStyleTargets.cmake
    NAMESPACE Zero::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZeroStyle
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ZeroStyleConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ZeroStyleConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZeroStyle
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ZeroStyleConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ZeroStyleConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ZeroStyleConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZeroStyle
)
