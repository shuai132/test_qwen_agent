cmake_minimum_required(VERSION 3.20)
project(agent_sdk VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译器特定选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# 查找Asio库
# 注意：Asio是头文件库，通常不需要链接
find_path(ASIO_INCLUDE_DIR asio.hpp
    PATHS /usr/include
          /usr/local/include
          /opt/homebrew/include  # macOS Homebrew
          ${PROJECT_SOURCE_DIR}/third_party/asio/include
)

if(ASIO_INCLUDE_DIR)
    include_directories(${ASIO_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Asio headers not found. Please install Asio library or download it to third_party/asio/include/")
endif()

# 包含目录
include_directories(include)

# 定义源文件
set(AGENT_SDK_SOURCES
    src/agent.cpp
    src/agent_loop.cpp
    src/message.cpp
    src/tool.cpp
    src/session.cpp
    src/mcp_client.cpp
    src/subagent.cpp
    src/context_manager.cpp
    src/tools/bash_tool.cpp
    src/tools/read_tool.cpp
    src/tools/glob_tool.cpp
    src/tools/grep_tool.cpp
    src/tools/edit_tool.cpp
    src/tools/write_tool.cpp
    src/tools/task_tool.cpp
    src/tools/skill_tool.cpp
    src/tools/web_search_tool.cpp
    src/tools/web_fetch_tool.cpp
    src/tools/code_search_tool.cpp
    src/tools/apply_patch_tool.cpp
    src/tools/question_tool.cpp
    src/tools/built_in_tools.cpp
)

# 创建库
add_library(agent_sdk ${AGENT_SDK_SOURCES})

# 设置库的公共包含目录
target_include_directories(agent_sdk PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 设置库的私有包含目录
target_include_directories(agent_sdk PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接库（如果需要其他库的话）
# target_link_libraries(agent_sdk )

# Google Test
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    
    # 查找或下载Google Test
    include(FetchContent)
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
    )
    
    # 设置googletest为不构建示例和额外的测试
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # 创建测试可执行文件
    add_executable(agent_sdk_tests 
        tests/test_main.cpp
    )
    
    # 链接测试库
    target_link_libraries(agent_sdk_tests 
        agent_sdk
        gtest_main
    )
    
    # 设置测试包含目录
    target_include_directories(agent_sdk_tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    
    # 添加测试可执行文件
    add_executable(test_tools
        tests/test_tools.cpp
    )

    target_link_libraries(test_tools
        agent_sdk
        gtest_main
    )

    target_include_directories(test_tools PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    # 添加测试命令
    add_test(NAME agent_sdk_unit_tests
             COMMAND agent_sdk_tests
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

# 示例应用程序
option(BUILD_EXAMPLES "Build examples" ON)

if(BUILD_EXAMPLES)
    add_executable(simple_agent_example
        examples/simple_agent.cpp
    )

    add_executable(tool_call_example
        examples/tool_call_example.cpp
    )

    add_executable(basic_tool_usage
        examples/basic_tool_usage.cpp
    )

    add_executable(built_in_tools_example
        examples/built_in_tools_example.cpp
    )

    target_link_libraries(simple_agent_example agent_sdk)
    target_link_libraries(tool_call_example agent_sdk)
    target_link_libraries(basic_tool_usage agent_sdk)
    target_link_libraries(built_in_tools_example agent_sdk)

    target_include_directories(simple_agent_example PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_include_directories(tool_call_example PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_include_directories(basic_tool_usage PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_include_directories(built_in_tools_example PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

# 安装规则
install(TARGETS agent_sdk
    EXPORT agent_sdk_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT agent_sdk_targets
    FILE agent_sdk-config.cmake
    NAMESPACE agent_sdk::
    DESTINATION lib/cmake/agent_sdk
)

# 导出目标
export(EXPORT agent_sdk_targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/agent_sdk-config.cmake
    NAMESPACE agent_sdk::
)

# 包配置
configure_file(cmake/agent_sdk-config-version.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/agent_sdk-config-version.cmake
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/agent_sdk-config-version.cmake
    DESTINATION lib/cmake/agent_sdk
)