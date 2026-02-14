#ifndef AGENT_SDK_BASH_TOOL_H
#define AGENT_SDK_BASH_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Bash tool for executing shell commands
 */
class BashTool {
public:
    /**
     * @brief Create a BashTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Execute a bash command
     * @param command The command to execute
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(const std::string& command, const ToolContext& context);
};

} // namespace openagent

#endif // AGENT_SDK_BASH_TOOL_H