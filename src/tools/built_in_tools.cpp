#include "agent_sdk/tools/built_in_tools.h"
#include "agent_sdk/tools/bash_tool.h"
#include "agent_sdk/tools/read_tool.h"
#include "agent_sdk/tools/glob_tool.h"
#include "agent_sdk/tools/grep_tool.h"
#include "agent_sdk/tools/edit_tool.h"
#include "agent_sdk/tools/write_tool.h"
#include "agent_sdk/tools/task_tool.h"
#include "agent_sdk/tools/skill_tool.h"
#include "agent_sdk/tools/web_search_tool.h"
#include "agent_sdk/tools/web_fetch_tool.h"
#include "agent_sdk/tools/code_search_tool.h"
#include "agent_sdk/tools/apply_patch_tool.h"
#include "agent_sdk/tools/question_tool.h"

namespace openagent {

void BuiltInTools::registerAll(std::shared_ptr<ToolRegistry> registry) {
    registry->registerTool(createInvalidTool());
    registry->registerTool(createQuestionTool());
    registry->registerTool(createBashTool());
    registry->registerTool(createReadTool());
    registry->registerTool(createGlobTool());
    registry->registerTool(createGrepTool());
    registry->registerTool(createEditTool());
    registry->registerTool(createWriteTool());
    registry->registerTool(createTaskTool());
    registry->registerTool(createSkillTool());
    registry->registerTool(createWebSearchTool());
    registry->registerTool(createWebFetchTool());
    registry->registerTool(createCodeSearchTool());
    registry->registerTool(createApplyPatchTool());
}

Tool::Ptr BuiltInTools::createInvalidTool() {
    // For now, we'll create a simple placeholder for the invalid tool
    Tool::Definition def;
    def.id = "invalid";
    def.description = "Handles invalid tool calls by providing helpful suggestions";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "original_tool": {
                "type": "string",
                "description": "The originally called tool that was invalid"
            },
            "suggestion": {
                "type": "string",
                "description": "Suggested alternative tool or correction"
            }
        },
        "required": ["original_tool", "suggestion"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        ToolExecutionResult res;
        res.title = "Invalid Tool Handler";
        res.output = "Handled invalid tool call. See suggestions for alternatives.";
        res.success = true;
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

Tool::Ptr BuiltInTools::createQuestionTool() {
    return QuestionTool::create();
}

Tool::Ptr BuiltInTools::createBashTool() {
    return BashTool::create();
}

Tool::Ptr BuiltInTools::createReadTool() {
    return ReadTool::create();
}

Tool::Ptr BuiltInTools::createGlobTool() {
    return GlobTool::create();
}

Tool::Ptr BuiltInTools::createGrepTool() {
    return GrepTool::create();
}

Tool::Ptr BuiltInTools::createEditTool() {
    return EditTool::create();
}

Tool::Ptr BuiltInTools::createWriteTool() {
    return WriteTool::create();
}

Tool::Ptr BuiltInTools::createTaskTool() {
    return TaskTool::create();
}

Tool::Ptr BuiltInTools::createSkillTool() {
    return SkillTool::create();
}

Tool::Ptr BuiltInTools::createWebSearchTool() {
    return WebSearchTool::create();
}

Tool::Ptr BuiltInTools::createWebFetchTool() {
    return WebFetchTool::create();
}

Tool::Ptr BuiltInTools::createCodeSearchTool() {
    return CodeSearchTool::create();
}

Tool::Ptr BuiltInTools::createApplyPatchTool() {
    return ApplyPatchTool::create();
}

} // namespace openagent