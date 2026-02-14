#ifndef AGENT_SDK_BUILT_IN_TOOLS_H
#define AGENT_SDK_BUILT_IN_TOOLS_H

#include "agent_sdk/tool.h"
#include <memory>

namespace openagent {

/**
 * @brief Built-in tools factory
 * 
 * Creates and registers all built-in tools for the agent
 */
class BuiltInTools {
public:
    /**
     * @brief Register all built-in tools with the registry
     * @param registry Tool registry to register tools with
     */
    static void registerAll(std::shared_ptr<ToolRegistry> registry);

    // Individual tool creation methods
    static Tool::Ptr createInvalidTool();
    static Tool::Ptr createQuestionTool();
    static Tool::Ptr createBashTool();
    static Tool::Ptr createReadTool();
    static Tool::Ptr createGlobTool();
    static Tool::Ptr createGrepTool();
    static Tool::Ptr createEditTool();
    static Tool::Ptr createWriteTool();
    static Tool::Ptr createTaskTool();
    static Tool::Ptr createSkillTool();
    static Tool::Ptr createWebSearchTool();
    static Tool::Ptr createWebFetchTool();
    static Tool::Ptr createCodeSearchTool();
    static Tool::Ptr createApplyPatchTool();
};

} // namespace openagent

#endif // AGENT_SDK_BUILT_IN_TOOLS_H