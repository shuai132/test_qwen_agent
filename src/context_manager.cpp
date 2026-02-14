#include "agent_sdk/context_manager.h"

#include <algorithm>
#include <set>

namespace openagent {

class ContextManager::Impl {
 public:
};

ContextManager::Ptr ContextManager::getInstance() {
  static ContextManager::Ptr instance = []() {
    return std::shared_ptr<ContextManager>(new ContextManager());
  }();
  return instance;
}

std::vector<Message::Ptr> ContextManager::filterCompacted(const std::vector<Message::Ptr>& messages) const {
  std::vector<Message::Ptr> result;
  std::set<std::string> completed;  // 存储已完成的摘要对应的parentID

  // 从最新的消息开始向前扫描
  for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
    auto msg = *it;
    result.push_back(msg);

    // 如果遇到assistant消息且是摘要类型且已完成
    if (msg->getType() == MessageType::ASSISTANT && msg->getFinishReason().has_value() && msg->getFinishReason().value() == FinishReason::STOP) {
      // 在实际实现中，这里会检查是否是摘要消息
      // completed.insert(msg->getParentID()); // 假设有getParentID方法
      break;  // 简化处理，遇到完成的assistant消息就停止
    }

    // 检查是否有压缩相关的元数据
    auto metadata = msg->getMetadata();
    auto compaction_it = metadata.find("compaction");
    if (compaction_it != metadata.end()) {
      // 如果这个压缩任务已完成，则后续的老消息会被过滤掉
      if (completed.count(compaction_it->second) > 0) {
        break;  // 截断，不再包含更早的消息
      }
    }
  }

  // 反转结果以恢复正确的时间顺序
  std::reverse(result.begin(), result.end());
  return result;
}

bool ContextManager::isOverflow(const Session::Ptr& session) const {
  return session->getTokenCount() > session->getConfig().max_context_tokens;
}

CompactionInfo ContextManager::createCompaction(const Session::Ptr& session, CompactionType type) {
  // 在会话中创建压缩标记
  // 这将在下一次循环中被处理
  return session->compact(type);
}

bool ContextManager::processCompaction(const Session::Ptr& session) {
  // 在实际实现中，这里会使用LLM来生成上下文摘要
  // 目前我们使用Session类中已实现的简化压缩逻辑

  // 触发压缩
  auto compaction_info = session->compact(CompactionType::CONTEXT_OVERFLOW);

  // 添加一个继续消息提示
  Message::Ptr continue_msg = std::make_shared<Message>(MessageType::USER, "Continue if you have next steps");
  continue_msg->setMetadata({{"synthetic", "true"}});  // 标记为系统生成
  session->addMessage(continue_msg);

  return true;
}

// Constructor for ContextManager
ContextManager::ContextManager() : pimpl_(std::make_unique<Impl>()) {}

}  // namespace openagent