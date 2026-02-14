# Agent SDK 项目概述

## 项目目的

Agent SDK 是一个 C++ 库，旨在促进智能代理的创建和管理。它提供了一个全面的框架，用于构建能够与大语言模型交互、管理对话、执行工具以及处理复杂工作流（包括子代理和上下文管理）的代理。

## 架构

SDK 采用模块化架构，包含以下关键组件：

### 核心组件

- **Agent**: 表示智能代理的主要类，管理代理生命周期、工具和会话
- **Message**: 表示对话消息，支持不同类型（用户、助手、系统、工具调用、工具结果）
- **Tool**: 定义代理可以使用的可执行函数，带有参数的 JSON schema
- **Session**: 管理对话状态、消息历史和上下文溢出处理
- **AgentLoop**: 处理消息的主要执行循环，协调代理行为

### 高级特性

- **MCP Client**: 实现 Model Context Protocol，用于连接外部服务和工具
- **Subagent Manager**: 启用具有子代理的分层任务管理
- **Context Manager**: 处理上下文溢出检测和消息压缩
- **Tool Registry**: 集中管理可用工具

## 构建和运行

### 先决条件

- C++20 兼容编译器（GCC、Clang 或 MSVC）
- CMake 3.20 或更高版本
- Asio 库（仅头文件）

### 构建说明

```bash
mkdir build
cd build
cmake ..
make
```

### 构建选项

- `BUILD_TESTS`（默认开启）：使用 Google Test 构建单元测试
- `BUILD_EXAMPLES`（默认开启）：构建示例应用程序

### 运行测试

```bash
cd build
ctest
# 或直接运行测试可执行文件：
./bin/agent_sdk_tests
```

### 运行示例

```bash
cd build
./bin/simple_agent_example
```

## 关键特性

### 1. 消息处理

SDK 支持丰富的消息类型，包括：

- 用户和助手消息
- 系统消息
- 工具调用和结果
- 多模态内容支持

### 2. 工具集成

- 使用 JSON schema 验证定义自定义工具
- 同步或异步执行工具
- 自动工具注册和发现
- 通过 MCP（Model Context Protocol）支持外部工具

### 3. 上下文管理

- 自动上下文溢出检测
- 智能消息压缩和修剪
- 令牌计数和估算
- 会话状态管理

### 4. 子代理支持

- 分层任务分解
- 子会话管理
- 父子代理之间的任务协调

### 5. 配置管理

全面的配置选项，包括：

- 模型选择和 API 设置
- 上下文窗口限制
- 工具启用/禁用
- 超时和重试配置
- MCP 服务器设置

## 开发约定

### 代码风格

- C++20 标准及现代特性
- RAII（资源获取即初始化）模式
- 智能指针进行内存管理
- 需要时的线程安全实现

### 测试

- Google Test 框架用于单元测试
- 核心组件的全面测试覆盖
- 端到端功能的集成测试

### 错误处理

- 异常安全设计
- 适当的资源清理
- 故障时的优雅降级

## 示例用法

SDK 包含一个简单示例，演示基本用法：

```cpp
// 创建配置
Config config;
config.id = "simple_example_agent";
config.name = "Simple Example Agent";
config.model = "gpt-4";
config.api_key = "your-api-key";

// 创建代理
Agent agent(config);

// 创建一个简单工具
Tool::Definition echo_def;
echo_def.id = "echo_tool";
echo_def.description = "回显输入文本";
echo_def.parameters_schema = R"({
    "type": "object",
    "properties": {
        "text": {
            "type": "string",
            "description": "要回显的文本"
        }
    },
    "required": ["text"]
})";
echo_def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
    ToolExecutionResult result;
    result.title = "回显工具结果";
    result.output = "回显：" + args;
    result.success = true;
    return result;
};

// 注册工具
auto echo_tool = std::make_shared<Tool>(echo_def);
agent.addTool(echo_tool);

// 运行代理
agent.run("example_session_123");
```

## 项目结构

- `include/agent_sdk/` - 公共头文件
- `src/` - 源代码实现
- `examples/` - 使用示例
- `tests/` - 单元测试
- `cmake/` - CMake 模块
- `doc/` - 文档（如果存在）
