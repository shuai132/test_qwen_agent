#ifndef AGENT_SDK_WEB_SEARCH_TOOL_H
#define AGENT_SDK_WEB_SEARCH_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Web search tool for performing web searches
 */
class WebSearchTool {
public:
    /**
     * @brief Create a WebSearchTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Perform a web search
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

#endif // AGENT_SDK_WEB_SEARCH_TOOL_H