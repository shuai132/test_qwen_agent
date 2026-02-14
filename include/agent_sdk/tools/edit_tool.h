#ifndef AGENT_SDK_EDIT_TOOL_H
#define AGENT_SDK_EDIT_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Edit tool for modifying file contents
 */
class EditTool {
public:
    /**
     * @brief Create an EditTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Edit a file's content
     * @param filepath Path to the file to edit
     * @param old_content The content to replace
     * @param new_content The new content to insert
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(
        const std::string& filepath,
        const std::string& old_content,
        const std::string& new_content,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_EDIT_TOOL_H