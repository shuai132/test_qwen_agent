#ifndef AGENT_SDK_APPLY_PATCH_TOOL_H
#define AGENT_SDK_APPLY_PATCH_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Apply patch tool for applying patches in GPT format
 */
class ApplyPatchTool {
public:
    /**
     * @brief Create an ApplyPatchTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Apply a patch to a file
     * @param filepath Path to the file to patch
     * @param patch_content The patch content in GPT format
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(
        const std::string& filepath,
        const std::string& patch_content,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_APPLY_PATCH_TOOL_H