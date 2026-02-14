#ifndef AGENT_SDK_EXAMPLES_SIMPLE_AGENT_CPP
#define AGENT_SDK_EXAMPLES_SIMPLE_AGENT_CPP

#include <chrono>
#include <iostream>
#include <thread>

#include "agent_sdk/agent.h"
#include "agent_sdk/config.h"
#include "agent_sdk/message.h"
#include "agent_sdk/tool.h"

using namespace openagent;

int main() {
  std::cout << "Initializing OpenAgent SDK Example..." << std::endl;

  // 创建配置
  Config config;
  config.id = "simple_example_agent";
  config.name = "Simple Example Agent";
  config.description = "A simple example of using the OpenAgent SDK";
  config.model = "gpt-4";                    // 示例模型
  config.api_key = "dummy-key-for-example";  // 在实际应用中应使用真实密钥
  config.base_url = "https://api.openai.com/v1";

  // 创建Agent并保持其生命周期
  auto agent = std::make_shared<Agent>(config);

  // 创建一个简单的工具示例
  Tool::Definition echo_def;
  echo_def.id = "echo_tool";
  echo_def.description = "Echoes back the input text";
  echo_def.parameters_schema = R"({
        "type": "object",
        "properties": {
            "text": {
                "type": "string",
                "description": "The text to echo back"
            }
        },
        "required": ["text"]
    })";
  echo_def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
    std::cout << "Echo tool called with: " << args << std::endl;

    ToolExecutionResult result;
    result.title = "Echo Tool Result";
    result.output = "Echoed: " + args;
    result.success = true;

    return result;
  };

  // 注册工具
  auto echo_tool = std::make_shared<Tool>(echo_def);
  agent->addTool(echo_tool);

  std::cout << "Starting agent session..." << std::endl;

  // 启动Agent（在一个单独的线程中，因为run是阻塞的）
  std::thread agent_thread([agent]() {
    agent->run("example_session_123");
  });

  // 等待一段时间然后停止
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // 停止Agent
  agent->stop();

  if (agent_thread.joinable()) {
    agent_thread.join();
  }

  std::cout << "Agent example completed." << std::endl;

  return 0;
}

#endif  // AGENT_SDK_EXAMPLES_SIMPLE_AGENT_CPP