#include "agent_sdk/tools/edit_tool.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace openagent {

Tool::Ptr EditTool::create() {
    Tool::Definition def;
    def.id = "edit";
    def.description = "Edit a file by replacing content";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "filepath": {
                "type": "string",
                "description": "The path to the file to edit"
            },
            "old_content": {
                "type": "string",
                "description": "The content to replace"
            },
            "new_content": {
                "type": "string",
                "description": "The new content to insert"
            }
        },
        "required": ["filepath", "old_content", "new_content"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract filepath, old_content, and new_content
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
        
        // Find the old_content in the JSON string
        size_t old_content_start = args.find("\"old_content\":\"");
        std::string old_content;
        if (old_content_start != std::string::npos) {
            old_content_start += 15; // Length of "\"old_content\":\""
            size_t old_content_end = args.find("\"", old_content_start);
            if (old_content_end != std::string::npos) {
                old_content = args.substr(old_content_start, old_content_end - old_content_start);
            }
        }
        
        // Find the new_content in the JSON string
        size_t new_content_start = args.find("\"new_content\":\"");
        std::string new_content;
        if (new_content_start != std::string::npos) {
            new_content_start += 15; // Length of "\"new_content\":\""
            size_t new_content_end = args.find("\"", new_content_start);
            if (new_content_end != std::string::npos) {
                new_content = args.substr(new_content_start, new_content_end - new_content_start);
            }
        }
        
        // Sanitize the filepath to prevent directory traversal
        std::filesystem::path path(filepath);
        path = std::filesystem::weakly_canonical(path);
        
        // Check if the path is within the allowed directory
        std::filesystem::path current_dir = std::filesystem::current_path();
        if (path.string().find(current_dir.string()) != 0) {
            ToolExecutionResult res;
            res.title = "Edit File Failed";
            res.output = "Access denied: Cannot edit file outside of allowed directory";
            res.success = false;
            res.error = "Path traversal detected";
            return res;
        }
        
        // Read the current file content
        std::ifstream file(path);
        if (!file.is_open()) {
            ToolExecutionResult res;
            res.title = "Edit File Failed";
            res.output = "Could not open file: " + path.string();
            res.success = false;
            res.error = "File not found or inaccessible";
            return res;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        // Check if old_content exists in the file
        size_t pos = content.find(old_content);
        if (pos == std::string::npos) {
            ToolExecutionResult res;
            res.title = "Edit File Failed";
            res.output = "Content to replace not found in file: " + path.string();
            res.success = false;
            res.error = "Old content not found";
            return res;
        }
        
        // Replace the old content with new content
        content.replace(pos, old_content.length(), new_content);
        
        // Write the modified content back to the file
        std::ofstream out_file(path);
        if (!out_file.is_open()) {
            ToolExecutionResult res;
            res.title = "Edit File Failed";
            res.output = "Could not write to file: " + path.string();
            res.success = false;
            res.error = "Cannot write to file";
            return res;
        }
        
        out_file << content;
        out_file.close();
        
        ToolExecutionResult res;
        res.title = "File Edited Successfully";
        res.output = "Successfully replaced content in file: " + path.string();
        res.success = true;
        
        // Add metadata
        res.metadata["filepath"] = path.string();
        res.metadata["replaced_length"] = std::to_string(old_content.length());
        res.metadata["inserted_length"] = std::to_string(new_content.length());
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult EditTool::execute(
    const std::string& filepath,
    const std::string& old_content,
    const std::string& new_content,
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
        res.title = "Edit File Failed";
        res.output = "Access denied: Cannot edit file outside of allowed directory";
        res.success = false;
        res.error = "Path traversal detected";
        return res;
    }
    
    // Read the current file content
    std::ifstream file(path);
    if (!file.is_open()) {
        ToolExecutionResult res;
        res.title = "Edit File Failed";
        res.output = "Could not open file: " + path.string();
        res.success = false;
        res.error = "File not found or inaccessible";
        return res;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Check if old_content exists in the file
    size_t pos = content.find(old_content);
    if (pos == std::string::npos) {
        ToolExecutionResult res;
        res.title = "Edit File Failed";
        res.output = "Content to replace not found in file: " + path.string();
        res.success = false;
        res.error = "Old content not found";
        return res;
    }
    
    // Replace the old content with new content
    content.replace(pos, old_content.length(), new_content);
    
    // Write the modified content back to the file
    std::ofstream out_file(path);
    if (!out_file.is_open()) {
        ToolExecutionResult res;
        res.title = "Edit File Failed";
        res.output = "Could not write to file: " + path.string();
        res.success = false;
        res.error = "Cannot write to file";
        return res;
    }
    
    out_file << content;
    out_file.close();
    
    ToolExecutionResult res;
    res.title = "File Edited Successfully";
    res.output = "Successfully replaced content in file: " + path.string();
    res.success = true;
    
    // Add metadata
    res.metadata["filepath"] = path.string();
    res.metadata["replaced_length"] = std::to_string(old_content.length());
    res.metadata["inserted_length"] = std::to_string(new_content.length());
    
    return res;
}

} // namespace openagent