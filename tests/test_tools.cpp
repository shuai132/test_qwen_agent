#include "agent_sdk/tool.h"
#include "agent_sdk/tools/built_in_tools.h"
#include "agent_sdk/tools/read_tool.h"
#include <iostream>
#include <fstream>
#include <memory>

int main() {
    // Create a temporary file for testing the read tool
    std::ofstream test_file("test_file.txt");
    test_file << "This is a test file for the read tool.\n";
    test_file << "It contains multiple lines of text.\n";
    test_file << "The read tool should be able to read this content.\n";
    test_file.close();
    
    // Get the tool registry instance
    auto registry = openagent::ToolRegistry::getInstance();
    
    // Register all built-in tools
    openagent::BuiltInTools::registerAll(registry);
    
    // Get the read tool
    auto read_tool = registry->getToolById("read");
    if (read_tool) {
        std::cout << "Testing read tool..." << std::endl;
        
        // Create a simple context for the tool
        openagent::ToolContext context;
        context.session_id = "test_session";
        context.agent_id = "test_agent";
        
        // Execute the read tool to read our test file
        std::string args = R"({"filepath":"test_file.txt"})";
        auto result = read_tool->execute(args, context);
        
        std::cout << "Title: " << result.title << std::endl;
        std::cout << "Success: " << (result.success ? "yes" : "no") << std::endl;
        std::cout << "Output: " << result.output << std::endl;
        
        if (result.error) {
            std::cout << "Error: " << *result.error << std::endl;
        }
    } else {
        std::cout << "Read tool not found!" << std::endl;
    }
    
    // Clean up the test file
    std::remove("test_file.txt");
    
    return 0;
}