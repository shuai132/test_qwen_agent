#ifndef AGENT_SDK_TASK_TOOL_H
#define AGENT_SDK_TASK_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Task tool for managing sub-agent tasks
 */
class TaskTool {
public:
    /**
     * @brief Create a TaskTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Execute a sub-task
     * @param task_description Description of the task to execute
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(
        const std::string& task_description,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_TASK_TOOL_H