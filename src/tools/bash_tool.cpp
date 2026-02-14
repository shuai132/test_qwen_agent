#include "agent_sdk/tools/bash_tool.h"
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

namespace openagent {

Tool::Ptr BashTool::create() {
    Tool::Definition def;
    def.id = "bash";
    def.description = "Execute a shell command and return its output";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "command": {
                "type": "string",
                "description": "The shell command to execute"
            }
        },
        "required": ["command"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the command
        // In a real implementation, you would use a JSON parser
        // For simplicity, we'll extract the command directly from the JSON string
        std::string command = arguments;
        
        // Find the command in the JSON string
        size_t pos_start = command.find("\"command\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 11; // Length of "\"command\":\""
            size_t pos_end = command.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                command = command.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // Execute the command and capture output
        std::array<char, 128> buffer;
        std::string result;
        int exit_code = 0;
        
#ifdef _WIN32
        // Windows implementation
        FILE* pipe = _popen(command.c_str(), "r");
#else
        // Unix-like implementation
        FILE* pipe = popen(command.c_str(), "r");
#endif
        
        if (!pipe) {
            ToolExecutionResult res;
            res.title = "Bash Command Failed";
            res.output = "Failed to execute command: " + command;
            res.success = false;
            res.error = "Could not create pipe to execute command";
            return res;
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        
#ifdef _WIN32
        exit_code = _pclose(pipe);
#else
        exit_code = pclose(pipe);
#endif
        
        ToolExecutionResult res;
        res.title = "Bash Command Result";
        res.output = result.empty() ? "Command executed successfully (no output)" : result;
        res.success = (exit_code == 0);
        if (!res.success) {
            res.error = "Command exited with code: " + std::to_string(exit_code);
        }
        
        // Add metadata
        res.metadata["exit_code"] = std::to_string(exit_code);
        res.metadata["command"] = command;
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult BashTool::execute(const std::string& command, const ToolContext& context) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    Tool::Definition def;
    def.id = "bash";
    def.description = "Execute a shell command and return its output";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "command": {
                "type": "string",
                "description": "The shell command to execute"
            }
        },
        "required": ["command"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& ctx) {
        // Parse the arguments to extract the command
        std::string cmd = arguments;
        
        // Find the command in the JSON string
        size_t pos_start = cmd.find("\"command\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 11; // Length of "\"command\":\""
            size_t pos_end = cmd.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                cmd = cmd.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // Execute the command and capture output
        std::array<char, 128> buffer;
        std::string result;
        int exit_code = 0;
        
#ifdef _WIN32
        // Windows implementation
        FILE* pipe = _popen(cmd.c_str(), "r");
#else
        // Unix-like implementation
        FILE* pipe = popen(cmd.c_str(), "r");
#endif
        
        if (!pipe) {
            ToolExecutionResult res;
            res.title = "Bash Command Failed";
            res.output = "Failed to execute command: " + cmd;
            res.success = false;
            res.error = "Could not create pipe to execute command";
            return res;
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        
#ifdef _WIN32
        exit_code = _pclose(pipe);
#else
        exit_code = pclose(pipe);
#endif
        
        ToolExecutionResult res;
        res.title = "Bash Command Result";
        res.output = result.empty() ? "Command executed successfully (no output)" : result;
        res.success = (exit_code == 0);
        if (!res.success) {
            res.error = "Command exited with code: " + std::to_string(exit_code);
        }
        
        // Add metadata
        res.metadata["exit_code"] = std::to_string(exit_code);
        res.metadata["command"] = cmd;
        
        return res;
    };
    
    return def.execute_func(command, context);
}

} // namespace openagent