# C++ Agent SDK

## 项目概述

Agent SDK 是一个用 C++ 编写的库，旨在促进能够与语言模型交互并执行各种工具的智能代理的创建。SDK 提供了一个灵活的框架，用于构建能够处理复杂对话、管理上下文并执行工具来执行操作的代理。

### 主要特性

- **代理框架**: 用于创建和管理智能代理的核心基础设施
- **工具系统**: 可扩展的工具框架，内置常用操作的工具
- **消息处理**: 支持各种内容类型和工具调用的结构化消息系统
- **会话管理**: 具有可配置超时和上下文限制的健壮会话管理
- **MCP 集成**: 支持模型上下文协议 (MCP) 服务器
- **内置工具**: 用于文件操作、shell 命令、网络搜索等的预建工具

### 架构

SDK 由几个核心组件组成：

- **Agent**: 协调对话和工具执行的主要代理类
- **Message**: 表示支持文本、工具调用和结果的对话消息
- **Tool**: 用于创建具有 JSON 模式验证的可执行工具的抽象接口
- **Session**: 管理对话状态和上下文
- **Config**: 用于自定义代理行为的配置结构

### 内置工具

SDK 包括以下几种内置工具：

- `bash_tool`: 执行 shell 命令
- `read_tool`: 读取文件内容
- `write_tool`: 将内容写入文件
- `edit_tool`: 编辑文件内容
- `glob_tool`: 使用 glob 模式查找文件
- `grep_tool`: 在文件中搜索模式
- `web_search_tool`: 执行网络搜索
- `web_fetch_tool`: 从 URL 获取内容
- `task_tool`: 将任务委托给专业子代理
- `skill_tool`: 执行专业技能
- `question_tool`: 向用户提问
- `apply_patch_tool`: 应用补丁到文件
- `code_search_tool`: 搜索代码库

## 构建和运行

### 先决条件

- C++20 兼容编译器 (GCC 8+, Clang 7+, MSVC 2019+)
- CMake 3.20+
- Asio 库 (仅头文件)
- Git (用于获取依赖项)

### 构建说明

```bash
# 克隆仓库
git clone <repository-url>
cd agent-sdk

# 创建构建目录
mkdir build && cd build

# 使用 CMake 配置
cmake ..

# 构建项目
make -j$(nproc)

# 可选地运行测试
ctest
```

### 使用 SDK

要在您的项目中使用 SDK，请链接到 `agent_sdk` 库并包含头文件：

```cpp
#include "agent_sdk/agent.h"
#include "agent_sdk/config.h"
#include "agent_sdk/message.h"
#include "agent_sdk/tool.h"

using namespace openagent;

// 创建配置
Config config;
config.id = "my_agent";
config.name = "My Agent";
config.model = "gpt-4";
config.api_key = "your-api-key";
config.base_url = "https://api.example.com/v1";

// 创建代理
auto agent = std::make_shared<Agent>(config);

// 添加工具
agent->addTool(BashTool::create());

// 运行代理
agent->run("session_id");
```

### 示例

SDK 在 `examples/` 目录中包含多个示例：

- `simple_agent.cpp`: 基本代理设置和使用
- `tool_call_example.cpp`: 演示工具调用功能
- `basic_tool_usage.cpp`: 显示如何使用内置工具
- `built_in_tools_example.cpp`: 使用各种内置工具的示例

## 开发规范

### 编码标准

- C++20 标准和现代 C++ 实践
- RAII (资源获取即初始化) 模式
- 智能指针进行内存管理
- 一致的命名约定 (方法使用驼峰命名法，变量使用下划线命名法)
- 全面的错误处理

### 测试

- 使用 Google Test 框架进行单元测试
- 测试位于 `tests/` 目录中
- 构建后使用 `ctest` 运行测试
- 所有新功能都应包含相应的测试

### 工具开发

创建新工具时：

1. 使用 JSON 模式定义工具参数
2. 实现执行逻辑
3. 遵循线程安全的执行模式
4. 优雅地处理错误并提供有意义的错误消息
5. 在结果中包含适当的元数据

## 配置选项

`Config` 结构提供了广泛的自定义选项：

- **模型设置**: API 密钥、基础 URL、模型选择
- **上下文管理**: 令牌限制、压缩阈值
- **超时**: 步骤和会话超时
- **工具配置**: 启用/禁用特定工具
- **重试逻辑**: 最大重试次数、延迟和退避因子
- **MCP 集成**: 服务器连接和超时

## 贡献

1. Fork 仓库
2. 创建功能分支
3. 按照编码标准进行更改
4. 为新功能添加测试
5. 提交带有清晰描述的拉取请求

## 许可证

[在此处指定许可证 - 在检查的文件中未找到]
