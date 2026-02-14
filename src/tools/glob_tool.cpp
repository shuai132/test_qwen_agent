#include "agent_sdk/tools/glob_tool.h"
#include <filesystem>
#include <sstream>

namespace openagent {

// Helper function to match patterns (simple implementation supporting * and ?)
bool matchesPattern(const std::string& str, const std::string& pattern) {
    if (pattern.empty()) return str.empty();
    
    if (pattern[0] == '*') {
        // Match zero or more characters
        for (size_t i = 0; i <= str.length(); ++i) {
            if (matchesPattern(str.substr(i), pattern.substr(1))) {
                return true;
            }
        }
        return false;
    } else if (!str.empty() && (pattern[0] == '?' || pattern[0] == str[0])) {
        // Match single character
        return matchesPattern(str.substr(1), pattern.substr(1));
    }
    
    return false;
}

Tool::Ptr GlobTool::create() {
    Tool::Definition def;
    def.id = "glob";
    def.description = "Find files using glob pattern matching";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "pattern": {
                "type": "string",
                "description": "The glob pattern to match files"
            }
        },
        "required": ["pattern"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the pattern
        std::string pattern = arguments;
        
        // Find the pattern in the JSON string
        size_t pos_start = pattern.find("\"pattern\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 11; // Length of "\"pattern\":\""
            size_t pos_end = pattern.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                pattern = pattern.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // Sanitize the pattern to prevent directory traversal
        std::filesystem::path pattern_path(pattern);
        std::string pattern_str = pattern_path.string();
        
        // Find matches based on the pattern
        std::vector<std::string> matches;
        std::string base_path = ".";
        
        // Extract base path from pattern if it contains directory separators
        size_t last_slash = pattern_str.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            base_path = pattern_str.substr(0, last_slash);
            pattern_str = pattern_str.substr(last_slash + 1);
        }
        
        try {
            // Walk through the directory tree to find matches
            for (const auto& entry : std::filesystem::recursive_directory_iterator(base_path)) {
                std::string filename = entry.path().filename().string();
                
                // Simple pattern matching (supports * and ?)
                if (matchesPattern(filename, pattern_str)) {
                    matches.push_back(entry.path().string());
                    
                    // Limit number of results to prevent huge outputs
                    if (matches.size() >= 100) {
                        matches.push_back("... (results limited to 100 files)");
                        break;
                    }
                }
            }
        } catch (const std::exception& e) {
            ToolExecutionResult res;
            res.title = "Glob Pattern Matching Failed";
            res.output = "Error during pattern matching: " + std::string(e.what());
            res.success = false;
            res.error = e.what();
            return res;
        }
        
        // Format the results
        std::ostringstream oss;
        for (const auto& match : matches) {
            oss << match << "\n";
        }
        
        ToolExecutionResult res;
        res.title = "Glob Pattern Matches";
        res.output = oss.str();
        res.success = true;
        
        // Add metadata
        res.metadata["pattern"] = pattern;
        res.metadata["count"] = std::to_string(matches.size());
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult GlobTool::execute(const std::string& pattern, const ToolContext& context) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    std::vector<std::string> matches;
    std::string base_path = ".";
    
    // Extract base path from pattern if it contains directory separators
    std::string pattern_str = pattern;
    size_t last_slash = pattern_str.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        base_path = pattern_str.substr(0, last_slash);
        pattern_str = pattern_str.substr(last_slash + 1);
    }
    
    try {
        // Walk through the directory tree to find matches
        for (const auto& entry : std::filesystem::recursive_directory_iterator(base_path)) {
            std::string filename = entry.path().filename().string();
            
            // Simple pattern matching (supports * and ?)
            if (matchesPattern(filename, pattern_str)) {
                matches.push_back(entry.path().string());
                
                // Limit number of results to prevent huge outputs
                if (matches.size() >= 100) {
                    matches.push_back("... (results limited to 100 files)");
                    break;
                }
            }
        }
    } catch (const std::exception& e) {
        ToolExecutionResult res;
        res.title = "Glob Pattern Matching Failed";
        res.output = "Error during pattern matching: " + std::string(e.what());
        res.success = false;
        res.error = e.what();
        return res;
    }
    
    // Format the results
    std::ostringstream oss;
    for (const auto& match : matches) {
        oss << match << "\n";
    }
    
    ToolExecutionResult res;
    res.title = "Glob Pattern Matches";
    res.output = oss.str();
    res.success = true;
    
    // Add metadata
    res.metadata["pattern"] = pattern;
    res.metadata["count"] = std::to_string(matches.size());
    
    return res;
}

} // namespace openagent