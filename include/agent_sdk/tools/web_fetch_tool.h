#ifndef AGENT_SDK_WEB_FETCH_TOOL_H
#define AGENT_SDK_WEB_FETCH_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Web fetch tool for retrieving content from URLs
 */
class WebFetchTool {
public:
    /**
     * @brief Create a WebFetchTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Fetch content from a URL
     * @param url The URL to fetch content from
     * @param context Tool execution context
     * @return Execution result containing fetched content
     */
    static ToolExecutionResult execute(
        const std::string& url,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_WEB_FETCH_TOOL_H