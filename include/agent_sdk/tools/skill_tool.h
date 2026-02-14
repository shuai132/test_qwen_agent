#ifndef AGENT_SDK_SKILL_TOOL_H
#define AGENT_SDK_SKILL_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Skill tool for loading and executing skills on demand
 */
class SkillTool {
public:
    /**
     * @brief Create a SkillTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Execute a skill
     * @param skill_name Name of the skill to execute
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(
        const std::string& skill_name,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_SKILL_TOOL_H