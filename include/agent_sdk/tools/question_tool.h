#ifndef AGENT_SDK_QUESTION_TOOL_H
#define AGENT_SDK_QUESTION_TOOL_H

#include "agent_sdk/tool.h"

namespace openagent {

/**
 * @brief Question tool for asking questions to the user
 */
class QuestionTool {
public:
    /**
     * @brief Create a QuestionTool instance
     * @return Shared pointer to the tool
     */
    static Tool::Ptr create();
    
    /**
     * @brief Ask a question to the user
     * @param question The question to ask
     * @param context Tool execution context
     * @return Execution result
     */
    static ToolExecutionResult execute(
        const std::string& question,
        const ToolContext& context
    );
};

} // namespace openagent

#endif // AGENT_SDK_QUESTION_TOOL_H