#include "agent_sdk/tools/question_tool.h"

namespace openagent {

Tool::Ptr QuestionTool::create() {
    Tool::Definition def;
    def.id = "question";
    def.description = "Ask a question to the user";
    def.parameters_schema = R"""({
        "type": "object",
        "properties": {
            "question": {
                "type": "string",
                "description": "The question to ask the user"
            }
        },
        "required": ["question"]
    })""";

    def.execute_func = [](const std::string& arguments, const ToolContext& context) {
        // Parse the arguments to extract the question
        std::string question = arguments;
        
        // Find the question in the JSON string
        size_t pos_start = question.find("\"question\":\"");
        if (pos_start != std::string::npos) {
            pos_start += 12; // Length of "\"question\":\""
            size_t pos_end = question.find("\"", pos_start);
            if (pos_end != std::string::npos) {
                question = question.substr(pos_start, pos_end - pos_start);
            }
        }
        
        // In a real implementation, this would trigger an interaction with the user
        // For now, we'll simulate the response
        ToolExecutionResult res;
        res.title = "Question Asked";
        res.output = "Question submitted to user: " + question + "\nWaiting for user response...";
        res.success = true;
        
        // Add metadata
        res.metadata["question"] = question;
        
        return res;
    };
    
    return std::make_shared<Tool>(def);
}

ToolExecutionResult QuestionTool::execute(
    const std::string& question,
    const ToolContext& context
) {
    // This is a simplified version - in practice, the create() method handles execution
    // This function would be called internally by the tool framework
    
    // In a real implementation, this would trigger an interaction with the user
    // For now, we'll simulate the response
    ToolExecutionResult res;
    res.title = "Question Asked";
    res.output = "Question submitted to user: " + question + "\nWaiting for user response...";
    res.success = true;
    
    // Add metadata
    res.metadata["question"] = question;
    
    return res;
}

} // namespace openagent