#include "agent_sdk/tool.h"
#include "agent_sdk/tools/built_in_tools.h"
#include <iostream>
#include <memory>

int main() {
    // Get the tool registry instance
    auto registry = openagent::ToolRegistry::getInstance();
    
    // Register all built-in tools
    openagent::BuiltInTools::registerAll(registry);
    
    // Example: Get a specific tool (e.g., bash tool)
    auto bash_tool = registry->getToolById("bash");
    if (bash_tool) {
        std::cout << "Found tool: " << bash_tool->getId() << std::endl;
        std::cout << "Description: " << bash_tool->getDescription() << std::endl;
        
        // Create a simple context for the tool
        openagent::ToolContext context;
        context.session_id = "test_session";
        context.agent_id = "test_agent";
        
        // Example of how to execute the tool (this would require proper JSON arguments)
        // openagent::ToolExecutionResult result = bash_tool->execute(R"({"command":"echo Hello World"})", context);
    }
    
    // List all registered tools
    std::cout << "\nAll registered tools:" << std::endl;
    auto all_tools = registry->getAllTools();
    for (const auto& tool : all_tools) {
        std::cout << "- " << tool->getId() << ": " << tool->getDescription() << std::endl;
    }
    
    std::cout << "\nBuilt-in tools have been successfully registered!" << std::endl;
    
    return 0;
}