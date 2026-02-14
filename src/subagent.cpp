#include "agent_sdk/subagent.h"

#include <iostream>

#include "agent_sdk/agent.h"

namespace openagent {

// SubagentManager implementation
class SubagentManager::Impl {
 public:
  std::vector<SubagentTask> pending_tasks_;
  mutable std::mutex mutex_;
};

SubagentManager::Ptr SubagentManager::getInstance() {
  static SubagentManager::Ptr instance = []() {
    return std::shared_ptr<SubagentManager>(new SubagentManager());
  }();
  return instance;
}

Session::Ptr SubagentManager::createSubagentSession(const Session::Ptr& parent_session, const Config& agent_config,
                                                    const std::string& task_description) {
  // 创建一个新的会话ID，关联到父会话
  std::string sub_session_id = parent_session->getId() + "_sub_" + std::to_string(pimpl_->pending_tasks_.size() + 1);

  // 创建子会话
  auto sub_session = std::make_shared<Session>(sub_session_id, agent_config);

  // 添加任务描述作为系统消息
  Message::Ptr task_msg = std::make_shared<Message>(MessageType::SYSTEM, task_description);
  sub_session->addMessage(task_msg);

  return sub_session;
}

void SubagentManager::addPendingTask(const SubagentTask& task) {
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);
  pimpl_->pending_tasks_.push_back(task);
}

std::vector<SubagentTask> SubagentManager::getPendingTasks() const {
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);
  return pimpl_->pending_tasks_;
}

void SubagentManager::processPendingTasks(const Session::Ptr& parent_session) {
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);

  if (pimpl_->pending_tasks_.empty()) {
    return;
  }

  // 处理第一个待处理任务
  auto task = pimpl_->pending_tasks_.front();
  pimpl_->pending_tasks_.erase(pimpl_->pending_tasks_.begin());

  // 更新任务状态
  task.status = SubagentTaskStatus::RUNNING;
  task.created_at = std::chrono::system_clock::now();

  try {
    // 创建子代理配置
    Config sub_agent_config = parent_session->getConfig();
    sub_agent_config.id = task.agent_name + "_subagent";

    // 创建子会话
    auto sub_session = createSubagentSession(parent_session, sub_agent_config, task.description);

    // 创建子代理并运行
    Agent sub_agent(sub_agent_config);

    // 添加任何必要的工具到子代理
    // 在实际实现中，可能会根据任务类型限制可用工具

    // 运行子代理
    sub_agent.run(sub_session->getId());

    // 任务完成
    task.status = SubagentTaskStatus::COMPLETED;
    task.completed_at = std::chrono::system_clock::now();
    task.result = "Subagent task completed successfully";

    // 将子会话的结果合并回父会话
    auto sub_messages = sub_session->getMessages();
    for (const auto& msg : sub_messages) {
      parent_session->addMessage(msg);
    }

  } catch (const std::exception& e) {
    task.status = SubagentTaskStatus::FAILED;
    task.completed_at = std::chrono::system_clock::now();
    task.result = std::string("Subagent task failed: ") + e.what();
  }

  // 在实际实现中，可能需要将完成的任务存储起来供后续参考
  std::cout << "Processed subagent task: " << task.description
            << ", status: " << (task.status == SubagentTaskStatus::COMPLETED ? "SUCCESS" : "FAILED") << std::endl;
}

bool SubagentManager::hasPendingTasks() const {
  std::lock_guard<std::mutex> lock(pimpl_->mutex_);
  return !pimpl_->pending_tasks_.empty();
}

// Constructor for SubagentManager
SubagentManager::SubagentManager() : pimpl_(std::make_unique<Impl>()) {}

}  // namespace openagent