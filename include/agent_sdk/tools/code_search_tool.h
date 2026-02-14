#ifndef AGENT_SDK_CODE_SEARCH_TOOL_H
#define AGENT_SDK_CODE_SEARCH_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Code search tool for searching codebases
 */
class CodeSearchTool {
public:
    /**
     * @brief Create a CodeSearchTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Search in the codebase
     * @param query The search query
     * @param context Tool execution context
     * @return Execution result containing search results
     */
    static ToolExecutionResult execute(
        const std::string& query,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_CODE_SEARCH_TOOL_H