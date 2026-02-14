#include "agent_sdk/tools/task_tool.h"
#include "agent_sdk/session.h"

namespace openagent {

Tool::Ptr TaskTool::create() {
    Tool::Definition def;
    def.id = "task";
    def.description = "Execute a sub-task or delegate to another agent";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "task_description": {
                "type": "string",
                "description": "Description of the task to execute"
            }
        },
        "required": ["task_description"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the task description
        std::string task_description = arguments;
        
        // Find the task description in the JSON string
        size_t pos_start = task_description.find("\"task_description\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 19; // Length of "\"task_description\":\""
            size_t pos_end = task_description.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                task_description = task_description.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // In a real implementation, this would create a sub-session and execute the task
        // For now, we'll simulate the execution
        ToolExecutionResult res;
        res.title = "Task Executed";
        res.output = "Sub-task initiated: " + task_description + "\nTask execution in progress...";
        res.success = true;
        
        // Add metadata
        res.metadata["task_description"] = task_description;
        res.metadata["session_id"] = context.session_id;
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult TaskTool::execute(
    const std::string& task_description,
    const ToolContext& context
) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // In a real implementation, this would create a sub-session and execute the task
    // For now, we'll simulate the execution
    ToolExecutionResult res;
    res.title = "Task Executed";
    res.output = "Sub-task initiated: " + task_description + "\nTask execution in progress...";
    res.success = true;
    
    // Add metadata
    res.metadata["task_description"] = task_description;
    res.metadata["session_id"] = context.session_id;
    
    return res;
}

} // namespace openagent