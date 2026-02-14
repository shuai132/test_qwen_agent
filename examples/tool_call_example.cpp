#include <chrono>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>

#include "agent_sdk/agent.h"
#include "agent_sdk/config.h"
#include "agent_sdk/message.h"
#include "agent_sdk/tool.h"

using namespace openagent;
using json = nlohmann::json;

int main() {
    std::cout << "Initializing OpenAgent SDK Tool Call Example..." << std::endl;

    // 创建配置
    Config config;
    config.id = "tool_call_example_agent";
    config.name = "Tool Call Example Agent";
    config.description = "An example demonstrating tool calls in the OpenAgent SDK";
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

        // 解析参数
        json params;
        try {
            params = json::parse(args);
        } catch (const std::exception& e) {
            ToolExecutionResult result;
            result.title = "Echo Tool Error";
            result.output = "Error parsing arguments: " + std::string(e.what());
            result.success = false;
            result.error = e.what();
            return result;
        }

        std::string text = params.value("text", "No text provided");

        ToolExecutionResult result;
        result.title = "Echo Tool Result";
        result.output = "Echoed: " + text;
        result.success = true;

        std::cout << "[Echo Tool] Returning: " << result.output << std::endl;
        return result;
    };

    // 创建一个计算器工具
    Tool::Definition calc_def;
    calc_def.id = "calculator";
    calc_def.description = "Performs basic arithmetic operations (add, subtract, multiply, divide)";
    calc_def.parameters_schema = R"({
        "type": "object",
        "properties": {
            "operation": {
                "type": "string",
                "enum": ["add", "subtract", "multiply", "divide"],
                "description": "The operation to perform"
            },
            "a": {
                "type": "number",
                "description": "The first operand"
            },
            "b": {
                "type": "number",
                "description": "The second operand"
            }
        },
        "required": ["operation", "a", "b"]
    })";
    calc_def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
        std::cout << "[Calculator] Called with args: " << args << std::endl;

        json params;
        try {
            params = json::parse(args);
        } catch (const std::exception& e) {
            ToolExecutionResult result;
            result.title = "Calculator Error";
            result.output = "Error parsing arguments: " + std::string(e.what());
            result.success = false;
            result.error = e.what();
            return result;
        }

        std::string operation = params.value("operation", "");
        double a = params.value("a", 0.0);
        double b = params.value("b", 0.0);
        
        double result_val = 0.0;
        std::string output;
        
        if (operation == "add") {
            result_val = a + b;
            output = std::to_string(a) + " + " + std::to_string(b) + " = " + std::to_string(result_val);
        } else if (operation == "subtract") {
            result_val = a - b;
            output = std::to_string(a) + " - " + std::to_string(b) + " = " + std::to_string(result_val);
        } else if (operation == "multiply") {
            result_val = a * b;
            output = std::to_string(a) + " * " + std::to_string(b) + " = " + std::to_string(result_val);
        } else if (operation == "divide") {
            if (b == 0) {
                ToolExecutionResult result;
                result.title = "Calculator Error";
                result.output = "Division by zero error";
                result.success = false;
                result.error = "Cannot divide by zero";
                return result;
            }
            result_val = a / b;
            output = std::to_string(a) + " / " + std::to_string(b) + " = " + std::to_string(result_val);
        } else {
            ToolExecutionResult result;
            result.title = "Calculator Error";
            result.output = "Unknown operation: " + operation;
            result.success = false;
            result.error = "Invalid operation";
            return result;
        }

        ToolExecutionResult result;
        result.title = "Calculator Result";
        result.output = output;
        result.success = true;

        std::cout << "[Calculator] Returning: " << result.output << std::endl;
        return result;
    };

    // 创建一个获取当前时间的工具
    Tool::Definition time_def;
    time_def.id = "get_current_time";
    time_def.description = "Gets the current time";
    time_def.parameters_schema = R"({
        "type": "object",
        "properties": {},
        "required": []
    })";
    time_def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
        std::cout << "[Time Tool] Called with args: " << args << std::endl;

        auto now = std::time(nullptr);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

        ToolExecutionResult result;
        result.title = "Current Time";
        result.output = "Current time is: " + std::string(buffer);
        result.success = true;

        std::cout << "[Time Tool] Returning: " << result.output << std::endl;
        return result;
    };

    // 注册所有工具
    auto echo_tool = std::make_shared<Tool>(echo_def);
    auto calc_tool = std::make_shared<Tool>(calc_def);
    auto time_tool = std::make_shared<Tool>(time_def);
    
    agent->addTool(echo_tool);
    agent->addTool(calc_tool);
    agent->addTool(time_tool);

    std::cout << "Starting agent session with tools..." << std::endl;

    // 设置消息处理器以观察消息流
    agent->setMessageHandler([](const Message& msg) {
        std::cout << "Message received - Type: " << static_cast<int>(msg.getType()) 
                  << ", Content: " << msg.getContent() << std::endl;
        
        if (!msg.getToolCalls().empty()) {
            std::cout << "Tool calls detected:" << std::endl;
            for (const auto& tc : msg.getToolCalls()) {
                std::cout << "  - Tool: " << tc.name << ", Args: " << tc.arguments << std::endl;
            }
        }
        
        if (!msg.getToolResults().empty()) {
            std::cout << "Tool results detected:" << std::endl;
            for (const auto& tr : msg.getToolResults()) {
                std::cout << "  - Tool: " << tr.tool_name << ", Result: " << tr.result 
                          << ", Success: " << (tr.success ? "true" : "false") << std::endl;
            }
        }
    });

    // 启动Agent（在一个单独的线程中，因为run是阻塞的）
    std::thread agent_thread([agent]() {
        agent->run("tool_call_example_session");
    });

    // 等待一段时间让Agent运行
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 停止Agent
    agent->stop();

    if (agent_thread.joinable()) {
        agent_thread.join();
    }

    std::cout << "Tool call example completed." << std::endl;

    return 0;
}