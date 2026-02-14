#ifndef AGENT_SDK_WRITE_TOOL_H
#define AGENT_SDK_WRITE_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Write tool for creating new files
 */
class WriteTool {
public:
    /**
     * @brief Create a WriteTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Write content to a file
     * @param filepath Path to the file to write
     * @param content The content to write
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(
        const std::string& filepath,
        const std::string& content,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_WRITE_TOOL_H