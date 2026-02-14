#include "agent_sdk/tools/code_search_tool.h"
#include <filesystem>

namespace openagent {

Tool::Ptr CodeSearchTool::create() {
    Tool::Definition def;
    def.id = "code_search";
    def.description = "Search for code patterns in the codebase";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "query": {
                "type": "string",
                "description": "The code search query"
            }
        },
        "required": ["query"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the query
        std::string query = arguments;
        
        // Find the query in the JSON string
        size_t pos_start = query.find("\"query\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 9; // Length of "\"query\":\""
            size_t pos_end = query.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                query = query.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // In a real implementation, this would search the codebase
        // For now, we'll simulate the search results
        ToolExecutionResult res;
        res.title = "Code Search Results";
        res.output = "Search results for code query: '" + query + "'\n\n";
        res.output += "Simulated code search results:\n";
        res.output += "- Found reference to '" + query + "' in file1.cpp:10\n";
        res.output += "- Found reference to '" + query + "' in file2.h:25\n";
        res.output += "- Found reference to '" + query + "' in file3.cpp:42\n";
        res.success = true;
        
        // Add metadata
        res.metadata["query"] = query;
        res.metadata["session_id"] = context.session_id;
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult CodeSearchTool::execute(
    const std::string& query,
    const ToolContext& context
) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // In a real implementation, this would search the codebase
    // For now, we'll simulate the search results
    ToolExecutionResult res;
    res.title = "Code Search Results";
    res.output = "Search results for code query: '" + query + "'\n\n";
    res.output += "Simulated code search results:\n";
    res.output += "- Found reference to '" + query + "' in file1.cpp:10\n";
    res.output += "- Found reference to '" + query + "' in file2.h:25\n";
    res.output += "- Found reference to '" + query + "' in file3.cpp:42\n";
    res.success = true;
    
    // Add metadata
    res.metadata["query"] = query;
    res.metadata["session_id"] = context.session_id;
    
    return res;
}

} // namespace openagent