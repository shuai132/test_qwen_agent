#include "agent_sdk/tools/read_tool.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace openagent {

Tool::Ptr ReadTool::create() {
    Tool::Definition def;
    def.id = "read";
    def.description = "Read the content of a file";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "filepath": {
                "type": "string",
                "description": "The path to the file to read"
            }
        },
        "required": ["filepath"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the filepath
        std::string filepath = arguments;
        
        // Find the filepath in the JSON string
        size_t pos_start = filepath.find("\"filepath\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 12; // Length of "\"filepath\":\""
            size_t pos_end = filepath.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                filepath = filepath.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // Sanitize the filepath to prevent directory traversal
        std::filesystem::path path(filepath);
        path = std::filesystem::weakly_canonical(path);
        
        // Check if the path is within the allowed directory (e.g., current working directory)
        std::filesystem::path current_dir = std::filesystem::current_path();
        if (path.string().find(current_dir.string()) != 0) {
            ToolExecutionResult res;
            res.title = "Read File Failed";
            res.output = "Access denied: Cannot read file outside of allowed directory";
            res.success = false;
            res.error = "Path traversal detected";
            return res;
        }
        
        // Read the file content
        std::ifstream file(path);
        if (!file.is_open()) {
            ToolExecutionResult res;
            res.title = "Read File Failed";
            res.output = "Could not open file: " + path.string();
            res.success = false;
            res.error = "File not found or inaccessible";
            return res;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Limit the content size to prevent huge outputs
        const size_t MAX_SIZE = 50 * 1024; // 50KB
        if (content.size() > MAX_SIZE) {
            content = content.substr(0, MAX_SIZE) + "\n... (content truncated)";
        }
        
        ToolExecutionResult res;
        res.title = "File Content";
        res.output = content;
        res.success = true;
        
        // Add metadata
        res.metadata["filepath"] = path.string();
        res.metadata["size"] = std::to_string(content.size());
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult ReadTool::execute(const std::string& filepath, const ToolContext& context) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    std::filesystem::path path(filepath);
    path = std::filesystem::weakly_canonical(path);
    
    // Check if the path is within the allowed directory
    std::filesystem::path current_dir = std::filesystem::current_path();
    if (path.string().find(current_dir.string()) != 0) {
        ToolExecutionResult res;
        res.title = "Read File Failed";
        res.output = "Access denied: Cannot read file outside of allowed directory";
        res.success = false;
        res.error = "Path traversal detected";
        return res;
    }
    
    // Read the file content
    std::ifstream file(path);
    if (!file.is_open()) {
        ToolExecutionResult res;
        res.title = "Read File Failed";
        res.output = "Could not open file: " + path.string();
        res.success = false;
        res.error = "File not found or inaccessible";
        return res;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Limit the content size to prevent huge outputs
    const size_t MAX_SIZE = 50 * 1024; // 50KB
    if (content.size() > MAX_SIZE) {
        content = content.substr(0, MAX_SIZE) + "\n... (content truncated)";
    }
    
    ToolExecutionResult res;
    res.title = "File Content";
    res.output = content;
    res.success = true;
    
    // Add metadata
    res.metadata["filepath"] = path.string();
    res.metadata["size"] = std::to_string(content.size());
    
    return res;
}

} // namespace openagent