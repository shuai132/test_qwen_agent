#ifndef AGENT_SDK_READ_TOOL_H
#define AGENT_SDK_READ_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Read tool for reading file contents
 */
class ReadTool {
public:
    /**
     * @brief Create a ReadTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Read a file's content
     * @param filepath Path to the file to read
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(const std::string& filepath, const ToolContext& context);
};

} // namespace openagent

#endif // AGENT_SDK_READ_TOOL_H