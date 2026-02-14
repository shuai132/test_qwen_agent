#ifndef AGENT_SDK_GLOB_TOOL_H
#define AGENT_SDK_GLOB_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Glob tool for finding files using patterns
 */
class GlobTool {
public:
    /**
     * @brief Create a GlobTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Find files using glob pattern
     * @param pattern The glob pattern to match
     * @param context Tool execution context
     * @return Execution result containing matched files
     */
    static ToolExecutionResult execute(const std::string& pattern, const ToolContext& context);
};

} // namespace openagent

#endif // AGENT_SDK_GLOB_TOOL_H