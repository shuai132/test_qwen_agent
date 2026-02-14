#include "agent_sdk/tools/web_fetch_tool.h"

namespace openagent {

Tool::Ptr WebFetchTool::create() {
    Tool::Definition def;
    def.id = "web_fetch";
    def.description = "Fetch content from a URL";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "url": {
                "type": "string",
                "description": "The URL to fetch content from"
            }
        },
        "required": ["url"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the URL
        std::string url = arguments;
        
        // Find the URL in the JSON string
        size_t pos_start = url.find("\"url\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 7; // Length of "\"url\":\""
            size_t pos_end = url.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                url = url.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // In a real implementation, this would fetch content from the URL
        // For now, we'll simulate the fetched content
        ToolExecutionResult res;
        res.title = "Web Fetch Results";
        res.output = "Fetched content from URL: " + url + "\n\n";
        res.output += "Simulated content for: " + url + "\n";
        res.output += "This is where the fetched content would appear...\n";
        res.success = true;
        
        // Add metadata
        res.metadata["url"] = url;
        res.metadata["session_id"] = context.session_id;
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult WebFetchTool::execute(
    const std::string& url,
    const ToolContext& context
) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // In a real implementation, this would fetch content from the URL
    // For now, we'll simulate the fetched content
    ToolExecutionResult res;
    res.title = "Web Fetch Results";
    res.output = "Fetched content from URL: " + url + "\n\n";
    res.output += "Simulated content for: " + url + "\n";
    res.output += "This is where the fetched content would appear...\n";
    res.success = true;
    
    // Add metadata
    res.metadata["url"] = url;
    res.metadata["session_id"] = context.session_id;
    
    return res;
}

} // namespace openagent