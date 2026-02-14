#include "agent_sdk/tools/write_tool.h"
#include <fstream>
#include <filesystem>

namespace openagent {

Tool::Ptr WriteTool::create() {
    Tool::Definition def;
    def.id = "write";
    def.description = "Write content to a file";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "filepath": {
                "type": "string",
                "description": "The path to the file to write"
            },
            "content": {
                "type": "string",
                "description": "The content to write to the file"
            }
        },
        "required": ["filepath", "content"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract filepath and content
        std::string args = arguments;
        
        // Find the filepath in the JSON string
        size_t filepath_start = args.find("\"filepath\":\"");
        std::string filepath;
        if (filepath_start != std::string::npos) {
            filepath_start += 12; // Length of "\"filepath\":\""
            size_t filepath_end = args.find("\"", filepath_start);
            if (filepath_end != std::string::npos) {
                filepath = args.substr(filepath_start, filepath_end - filepath_start);
            }
        }
        
        // Find the content in the JSON string
        size_t content_start = args.find("\"content\":\"");
        std::string content;
        if (content_start != std::string::npos) {
            content_start += 11; // Length of "\"content\":\""
            size_t content_end = args.find("\"", content_start);
            if (content_end != std::string::npos) {
                content = args.substr(content_start, content_end - content_start);
            }
        }
        
        // Sanitize the filepath to prevent directory traversal
        std::filesystem::path path(filepath);
        path = std::filesystem::weakly_canonical(path);
        
        // Check if the path is within the allowed directory
        std::filesystem::path current_dir = std::filesystem::current_path();
        if (path.string().find(current_dir.string()) != 0) {
            ToolExecutionResult res;
            res.title = "Write File Failed";
            res.output = "Access denied: Cannot write file outside of allowed directory";
            res.success = false;
            res.error = "Path traversal detected";
            return res;
        }
        
        // Create directory if it doesn't exist
        std::filesystem::path dir_path = path.parent_path();
        if (!dir_path.empty() && !std::filesystem::exists(dir_path)) {
            try {
                std::filesystem::create_directories(dir_path);
            } catch (const std::exception& e) {
                ToolExecutionResult res;
                res.title = "Write File Failed";
                res.output = "Could not create directory: " + dir_path.string() + ", error: " + e.what();
                res.success = false;
                res.error = e.what();
                return res;
            }
        }
        
        // Write the content to the file
        std::ofstream file(path);
        if (!file.is_open()) {
            ToolExecutionResult res;
            res.title = "Write File Failed";
            res.output = "Could not write to file: " + path.string();
            res.success = false;
            res.error = "Cannot write to file";
            return res;
        }
        
        file << content;
        file.close();
        
        ToolExecutionResult res;
        res.title = "File Written Successfully";
        res.output = "Successfully wrote content to file: " + path.string();
        res.success = true;
        
        // Add metadata
        res.metadata["filepath"] = path.string();
        res.metadata["written_bytes"] = std::to_string(content.length());
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult WriteTool::execute(
    const std::string& filepath,
    const std::string& content,
    const ToolContext& context
) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // Sanitize the filepath to prevent directory traversal
    std::filesystem::path path(filepath);
    path = std::filesystem::weakly_canonical(path);
    
    // Check if the path is within the allowed directory
    std::filesystem::path current_dir = std::filesystem::current_path();
    if (path.string().find(current_dir.string()) != 0) {
        ToolExecutionResult res;
        res.title = "Write File Failed";
        res.output = "Access denied: Cannot write file outside of allowed directory";
        res.success = false;
        res.error = "Path traversal detected";
        return res;
    }
    
    // Create directory if it doesn't exist
    std::filesystem::path dir_path = path.parent_path();
    if (!dir_path.empty() && !std::filesystem::exists(dir_path)) {
        try {
            std::filesystem::create_directories(dir_path);
        } catch (const std::exception& e) {
            ToolExecutionResult res;
            res.title = "Write File Failed";
            res.output = "Could not create directory: " + dir_path.string() + ", error: " + e.what();
            res.success = false;
            res.error = e.what();
            return res;
        }
    }
    
    // Write the content to the file
    std::ofstream file(path);
    if (!file.is_open()) {
        ToolExecutionResult res;
        res.title = "Write File Failed";
        res.output = "Could not write to file: " + path.string();
        res.success = false;
        res.error = "Cannot write to file";
        return res;
    }
    
    file << content;
    file.close();
    
    ToolExecutionResult res;
    res.title = "File Written Successfully";
    res.output = "Successfully wrote content to file: " + path.string();
    res.success = true;
    
    // Add metadata
    res.metadata["filepath"] = path.string();
    res.metadata["written_bytes"] = std::to_string(content.length());
    
    return res;
}

} // namespace openagent