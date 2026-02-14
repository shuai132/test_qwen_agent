#include "agent_sdk/tool.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "agent_sdk/session.h"

namespace openagent {

// Tool implementation
Tool::Tool(const Definition& definition) : definition_(definition) {}

const std::string& Tool::getId() const {
  return definition_.id;
}

const std::string& Tool::getDescription() const {
  return definition_.description;
}

const std::string& Tool::getParametersSchema() const {
  return definition_.parameters_schema;
}

ToolExecutionResult Tool::execute(const std::string& arguments, const ToolContext& context) const {
  if (definition_.execute_func) {
    return definition_.execute_func(arguments, context);
  }

  // 默认返回错误结果
  ToolExecutionResult result;
  result.title = "Execution Error";
  result.output = "No execution function defined for tool: " + definition_.id;
  result.success = false;
  result.error = "No execution function";
  return result;
}

// ToolRegistry implementation
class ToolRegistry::Impl {
 public:
  std::map<std::string, Tool::Ptr> tools_;
  mutable std::mutex mutex_;
};

ToolRegistry::Ptr ToolRegistry::getInstance() {
  static ToolRegistry::Ptr instance = []() {
    return std::shared_ptr<ToolRegistry>(new ToolRegistry());
  }();
  return instance;
}

void ToolRegistry::registerTool(const Tool::Ptr& tool) {
  if (tool) {
    std::lock_guard<std::mutex> lock(impl_->mutex_);
    impl_->tools_[tool->getId()] = tool;
  }
}

std::vector<Tool::Ptr> ToolRegistry::getAllTools() const {
  std::lock_guard<std::mutex> lock(impl_->mutex_);
  std::vector<Tool::Ptr> tools;
  for (const auto& pair : impl_->tools_) {
    tools.push_back(pair.second);
  }
  return tools;
}

Tool::Ptr ToolRegistry::getToolById(const std::string& id) const {
  std::lock_guard<std::mutex> lock(impl_->mutex_);
  auto it = impl_->tools_.find(id);
  if (it != impl_->tools_.end()) {
    return it->second;
  }
  return nullptr;
}

// Constructor for ToolRegistry
ToolRegistry::ToolRegistry() : impl_(std::make_unique<Impl>()) {}

}  // namespace openagent