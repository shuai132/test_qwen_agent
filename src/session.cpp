#include "agent_sdk/session.h"

#include <algorithm>
#include <numeric>

namespace openagent {

class Session::Impl {
 public:
  Impl(const std::string& id, const Config& config) : id_(id), config_(config), state_(SessionState::IDLE) {}

  void addMessage(const Message::Ptr& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.push_back(message);
  }

  std::vector<Message::Ptr> getMessages() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return messages_;
  }

  std::vector<Message::Ptr> getLastMessages(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (count >= messages_.size()) {
      return messages_;
    }

    std::vector<Message::Ptr> result;
    auto start_it = messages_.end() - count;
    for (auto it = start_it; it != messages_.end(); ++it) {
      result.push_back(*it);
    }
    return result;
  }

  const std::string& getId() const {
    return id_;
  }

  SessionState getState() const {
    return state_;
  }

  void setState(SessionState state) {
    state_ = state;
  }

  const Config& getConfig() const {
    return config_;
  }

  bool isContextOverflow() const {
    return getTokenCount() > config_.compaction_threshold;
  }

  CompactionInfo compact(CompactionType type) {
    std::lock_guard<std::mutex> lock(mutex_);

    CompactionInfo info;
    info.id = "compaction_" + std::to_string(compaction_history_.size() + 1);
    info.type = type;
    info.timestamp = std::chrono::system_clock::now();
    info.original_size = getTokenCount();

    // 简化的压缩逻辑：保留最近的几条消息，其他的标记为压缩
    // 在实际实现中，这里会使用LLM来生成摘要
    if (messages_.size() > 5) {  // 保留最后5条消息
      auto erase_end = messages_.end() - 5;
      messages_.erase(messages_.begin(), erase_end);
    }

    info.compressed_size = getTokenCount();
    info.summary = "Session compaction performed to reduce context size";

    compaction_history_.push_back(info);

    return info;
  }

  std::vector<CompactionInfo> getCompactionHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return compaction_history_;
  }

  void prune() {
    std::lock_guard<std::mutex> lock(mutex_);

    size_t current_tokens = getTokenCount();
    if (current_tokens <= config_.prune_protected_tokens + config_.prune_minimum_tokens) {
      // 不值得裁剪
      return;
    }

    // 从后往前遍历，删除超出保护范围的工具输出
    for (auto it = messages_.rbegin(); it != messages_.rend(); ++it) {
      auto msg = *it;
      if (current_tokens <= config_.prune_protected_tokens) {
        break;  // 已经在保护范围内
      }

      // 如果是工具结果消息，考虑裁剪
      if (msg->getType() == MessageType::TOOL_RESULT) {
        // 简化：直接移除整个消息以减少tokens
        // 在实际实现中，可能会保留部分结果而不是完全移除
        auto msg_tokens = estimateTokenCount(*msg);
        if (current_tokens - msg_tokens >= config_.prune_protected_tokens) {
          // 从原始消息列表中找到并删除这条消息
          auto remove_it = std::find_if(messages_.begin(), messages_.end(), [&msg](const Message::Ptr& m) {
            return m.get() == msg.get();
          });
          if (remove_it != messages_.end()) {
            current_tokens -= msg_tokens;
            messages_.erase(remove_it);
          }
        }
      }
    }
  }

  size_t getTokenCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = 0;
    for (const auto& msg : messages_) {
      total += estimateTokenCount(*msg);
    }
    return total;
  }

 private:
  size_t estimateTokenCount(const Message& msg) const {
    // 简单的token估算：字符数除以4（英文平均情况）
    size_t count = 0;

    for (const auto& content : msg.getContents()) {
      count += content.text.length() / 4;
    }

    // 工具调用和结果也计算在内
    for (const auto& call : msg.getToolCalls()) {
      count += call.name.length() / 4;
      count += call.arguments.length() / 4;
    }

    for (const auto& result : msg.getToolResults()) {
      count += result.result.length() / 4;
    }

    return count > 0 ? count : 1;  // 至少算作1个token
  }

  std::string id_;
  Config config_;
  SessionState state_;
  std::vector<Message::Ptr> messages_;
  std::vector<CompactionInfo> compaction_history_;
  mutable std::mutex mutex_;
};

// Session implementation
Session::Session(const std::string& id, const Config& config) : pimpl_(std::make_unique<Impl>(id, config)) {}

Session::~Session() = default;

void Session::addMessage(const Message::Ptr& message) {
  pimpl_->addMessage(message);
}

std::vector<Message::Ptr> Session::getMessages() const {
  return pimpl_->getMessages();
}

std::vector<Message::Ptr> Session::getLastMessages(size_t count) const {
  return pimpl_->getLastMessages(count);
}

const std::string& Session::getId() const {
  return pimpl_->getId();
}

SessionState Session::getState() const {
  return pimpl_->getState();
}

void Session::setState(SessionState state) {
  pimpl_->setState(state);
}

const Config& Session::getConfig() const {
  return pimpl_->getConfig();
}

bool Session::isContextOverflow() const {
  return pimpl_->isContextOverflow();
}

CompactionInfo Session::compact(CompactionType type) {
  return pimpl_->compact(type);
}

std::vector<CompactionInfo> Session::getCompactionHistory() const {
  return pimpl_->getCompactionHistory();
}

void Session::prune() {
  pimpl_->prune();
}

size_t Session::getTokenCount() const {
  return pimpl_->getTokenCount();
}

}  // namespace openagent