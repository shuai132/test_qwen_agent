#include "agent_sdk/tools/skill_tool.h"

namespace openagent {

Tool::Ptr SkillTool::create() {
    Tool::Definition def;
    def.id = "skill";
    def.description = "Load and execute a skill on demand";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "skill_name": {
                "type": "string",
                "description": "Name of the skill to execute"
            }
        },
        "required": ["skill_name"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the skill name
        std::string skill_name = arguments;
        
        // Find the skill name in the JSON string
        size_t pos_start = skill_name.find("\"skill_name\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 14; // Length of "\"skill_name\":\""
            size_t pos_end = skill_name.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                skill_name = skill_name.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // In a real implementation, this would load and execute the skill
        // For now, we'll simulate the execution
        ToolExecutionResult res;
        res.title = "Skill Executed";
        res.output = "Skill '" + skill_name + "' loaded and executed.\nSkill execution completed.";
        res.success = true;
        
        // Add metadata
        res.metadata["skill_name"] = skill_name;
        res.metadata["session_id"] = context.session_id;
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult SkillTool::execute(
    const std::string& skill_name,
    const ToolContext& context
) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // In a real implementation, this would load and execute the skill
    // For now, we'll simulate the execution
    ToolExecutionResult res;
    res.title = "Skill Executed";
    res.output = "Skill '" + skill_name + "' loaded and executed.\nSkill execution completed.";
    res.success = true;
    
    // Add metadata
    res.metadata["skill_name"] = skill_name;
    res.metadata["session_id"] = context.session_id;
    
    return res;
}

} // namespace openagent