#include "agent_sdk/tools/grep_tool.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace openagent {

Tool::Ptr GrepTool::create() {
    Tool::Definition def;
    def.id = "grep";
    def.description = "Search for a pattern in files";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "pattern": {
                "type": "string",
                "description": "The pattern to search for"
            },
            "filepath": {
                "type": "string",
                "description": "The path to the file(s) to search in"
            }
        },
        "required": ["pattern", "filepath"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the pattern and filepath
        std::string args = arguments;
        
        // Find the pattern in the JSON string
        size_t pattern_start = args.find("\"pattern\":\"");
        std::string pattern;
        if (pattern_start != std::string::npos) {
            pattern_start += 11; // Length of "\"pattern\":\""
            size_t pattern_end = args.find("\"", pattern_start);
            if (pattern_end != std::string::npos) {
                pattern = args.substr(pattern_start, pattern_end - pattern_start);
            }
        }
        
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
        
        // Sanitize the filepath to prevent directory traversal
        std::filesystem::path path(filepath);
        path = std::filesystem::weakly_canonical(path);
        
        // Check if the path is within the allowed directory
        std::filesystem::path current_dir = std::filesystem::current_path();
        if (path.string().find(current_dir.string()) != 0) {
            ToolExecutionResult res;
            res.title = "Grep Search Failed";
            res.output = "Access denied: Cannot search file outside of allowed directory";
            res.success = false;
            res.error = "Path traversal detected";
            return res;
        }
        
        // Compile the regex pattern
        std::regex regex_pattern;
        try {
            regex_pattern = std::regex(pattern);
        } catch (const std::regex_error& e) {
            ToolExecutionResult res;
            res.title = "Grep Search Failed";
            res.output = "Invalid regex pattern: " + std::string(e.what());
            res.success = false;
            res.error = "Regex compilation error";
            return res;
        }
        
        // Search in the file
        std::ifstream file(path);
        if (!file.is_open()) {
            ToolExecutionResult res;
            res.title = "Grep Search Failed";
            res.output = "Could not open file: " + path.string();
            res.success = false;
            res.error = "File not found or inaccessible";
            return res;
        }
        
        std::ostringstream result_stream;
        std::string line;
        int line_num = 0;
        int matches_found = 0;
        
        while (std::getline(file, line)) {
            ++line_num;
            
            if (std::regex_search(line, regex_pattern)) {
                result_stream << line_num << ": " << line << "\n";
                ++matches_found;
                
                // Limit number of results to prevent huge outputs
                if (matches_found >= 100) {
                    result_stream << "... (results limited to 100 matches)\n";
                    break;
                }
            }
        }
        
        ToolExecutionResult res;
        res.title = "Grep Search Results";
        res.output = result_stream.str().empty() ? 
                     "No matches found for pattern: " + pattern :
                     result_stream.str();
        res.success = true;
        
        // Add metadata
        res.metadata["pattern"] = pattern;
        res.metadata["filepath"] = path.string();
        res.metadata["matches_count"] = std::to_string(matches_found);
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult GrepTool::execute(const std::string& pattern, const std::string& filepath, const ToolContext& context) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // Sanitize the filepath to prevent directory traversal
    std::filesystem::path path(filepath);
    path = std::filesystem::weakly_canonical(path);
    
    // Check if the path is within the allowed directory
    std::filesystem::path current_dir = std::filesystem::current_path();
    if (path.string().find(current_dir.string()) != 0) {
        ToolExecutionResult res;
        res.title = "Grep Search Failed";
        res.output = "Access denied: Cannot search file outside of allowed directory";
        res.success = false;
        res.error = "Path traversal detected";
        return res;
    }
    
    // Compile the regex pattern
    std::regex regex_pattern;
    try {
        regex_pattern = std::regex(pattern);
    } catch (const std::regex_error& e) {
        ToolExecutionResult res;
        res.title = "Grep Search Failed";
        res.output = "Invalid regex pattern: " + std::string(e.what());
        res.success = false;
        res.error = "Regex compilation error";
        return res;
    }
    
    // Search in the file
    std::ifstream file(path);
    if (!file.is_open()) {
        ToolExecutionResult res;
        res.title = "Grep Search Failed";
        res.output = "Could not open file: " + path.string();
        res.success = false;
        res.error = "File not found or inaccessible";
        return res;
    }
    
    std::ostringstream result_stream;
    std::string line;
    int line_num = 0;
    int matches_found = 0;
    
    while (std::getline(file, line)) {
        ++line_num;
        
        if (std::regex_search(line, regex_pattern)) {
            result_stream << line_num << ": " << line << "\n";
            ++matches_found;
            
            // Limit number of results to prevent huge outputs
            if (matches_found >= 100) {
                result_stream << "... (results limited to 100 matches)\n";
                break;
            }
        }
    }
    
    ToolExecutionResult res;
    res.title = "Grep Search Results";
    res.output = result_stream.str().empty() ? 
                 "No matches found for pattern: " + pattern :
                 result_stream.str();
    res.success = true;
    
    // Add metadata
    res.metadata["pattern"] = pattern;
    res.metadata["filepath"] = path.string();
    res.metadata["matches_count"] = std::to_string(matches_found);
    
    return res;
}

} // namespace openagent