#include <chrono>
#include <iostream>
#include <thread>
#include <sstream>
#include <regex>

#include "agent_sdk/agent.h"
#include "agent_sdk/config.h"
#include "agent_sdk/message.h"
#include "agent_sdk/tool.h"

using namespace openagent;

int main() {
    std::cout << "Initializing OpenAgent SDK Basic Tool Usage Example..." << std::endl;

    // 创建配置
    Config config;
    config.id = "basic_tool_usage_agent";
    config.name = "Basic Tool Usage Agent";
    config.description = "A basic example demonstrating tool usage in the OpenAgent SDK";
    config.model = "gpt-4";                    
    config.api_key = "dummy-key-for-example";  
    config.base_url = "https://api.openai.com/v1";

    // 创建Agent并保持其生命周期
    auto agent = std::make_shared<Agent>(config);

    // 创建一个简单的回声工具
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
        std::cout << "[Echo Tool] Called with args: " << args << std::endl;

        // 使用正则表达式解析参数
        std::regex text_pattern("\"text\"\\s*:\\s*\"([^\"]*)\"");
        std::smatch matches;
        
        std::string text = "No text provided";
        if (std::regex_search(args, matches, text_pattern)) {
            text = matches[1].str();
        }

        ToolExecutionResult result;
        result.title = "Echo Tool Result";
        result.output = "Echoed: " + text;
        result.success = true;

        std::cout << "[Echo Tool] Returning: " << result.output << std::endl;
        return result;
    };

    // 注册工具
    auto echo_tool = std::make_shared<Tool>(echo_def);
    agent->addTool(echo_tool);

    std::cout << "Created and registered echo tool." << std::endl;

    // 演示如何手动调用工具
    std::cout << "\nDemonstrating manual tool execution:" << std::endl;
    
    ToolContext context;
    context.session_id = "demo_session";
    context.agent_id = "demo_agent";
    
    std::string tool_args = R"({"text": "Hello, World!"})";
    auto result = echo_tool->execute(tool_args, context);
    
    std::cout << "Tool execution result:" << std::endl;
    std::cout << "  Title: " << result.title << std::endl;
    std::cout << "  Output: " << result.output << std::endl;
    std::cout << "  Success: " << (result.success ? "true" : "false") << std::endl;
    if (result.error.has_value()) {
        std::cout << "  Error: " << result.error.value() << std::endl;
    }

    std::cout << "\nBasic tool usage example completed." << std::endl;

    return 0;
}