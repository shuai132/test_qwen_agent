#ifndef AGENT_SDK_GREP_TOOL_H
#define AGENT_SDK_GREP_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Grep tool for searching patterns in files
 */
class GrepTool {
public:
    /**
     * @brief Create a GrepTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Search for a pattern in files
     * @param pattern The pattern to search for
     * @param filepath Path to the file(s) to search in
     * @param context Tool execution context
     * @return Execution result containing matches
     */
    static ToolExecutionResult execute(const std::string& pattern, const std::string& filepath, const ToolContext& context);
};

} // namespace openagent

#endif // AGENT_SDK_GREP_TOOL_H