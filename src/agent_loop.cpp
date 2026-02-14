#include "agent_sdk/agent_loop.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#include "agent_sdk/agent.h"
#include "agent_sdk/message.h"
#include "agent_sdk/session.h"
#include "agent_sdk/subagent.h"
#include "agent_sdk/tool.h"

namespace openagent {

class AgentLoop::Impl {
 public:
  explicit Impl(const Agent::Ptr& agent) : agent_(agent), running_(false) {}

  ~Impl() {
    stop();
  }

  void run(const std::string& session_id) {
    if (running_) {
      return;
    }

    running_ = true;
    loop(session_id);
  }

  void stop() {
    running_ = false;
  }

  bool isRunning() const {
    return running_;
  }

  // 公有方法，供AgentLoop类的公共方法调用
  void loop(const std::string& session_id) {
    auto session = agent_->getSession();
    if (!session) {
      std::cerr << "Failed to get session: " << session_id << std::endl;
      return;
    }

    int step = 0;
    while (running_) {
      // 1. 从持久化存储加载消息（过滤已压缩的部分）
      auto messages = session->getMessages();  // In a real implementation, this would filter compacted messages

      // 2. 反向扫描找到 lastUser / lastAssistant / lastFinished / tasks
      Message::Ptr last_user_msg;
      Message::Ptr last_assistant_msg;
      Message::Ptr last_finished_msg;

      for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
        auto msg = *it;
        if (msg->getType() == MessageType::USER) {
          if (!last_user_msg) last_user_msg = msg;
        } else if (msg->getType() == MessageType::ASSISTANT) {
          if (!last_assistant_msg) last_assistant_msg = msg;
          if (msg->getFinishReason().has_value()) {
            if (!last_finished_msg) last_finished_msg = msg;
            break;  // Found the last finished message
          }
        }
      }

      // 3. 退出条件：最后一个 assistant 已完成且不是 tool-calls
      if (last_assistant_msg && last_assistant_msg->getFinishReason().has_value()) {
        auto finish_reason = last_assistant_msg->getFinishReason().value();
        if (finish_reason != FinishReason::TOOL_CALLS) {
          break;
        }
      }

      step++;

      // 4. 处理挂起的 subtask（子代理任务）
      if (handlePendingSubtasks(session)) {
        continue;
      }

      // 5. 处理挂起的 compaction（上下文压缩）
      if (handlePendingCompactions(session)) {
        continue;
      }

      // 6. 上下文溢出检测 → 自动触发 compaction
      if (checkContextOverflow(session)) {
        triggerCompaction(session);
        continue;
      }

      // 7. 正常处理：构建工具、调用 LLM
      auto result = processStep(session);

      if (result == AgentLoopResult::COMPACT) {
        triggerCompaction(session);  // 下一轮会进入压缩流程
      } else if (result == AgentLoopResult::STOP) {
        break;
      }
    }

    // 循环结束后裁剪老旧工具输出
    session->prune();
  }

  AgentLoopResult processStep(const Session::Ptr& session) {
    // 在实际实现中，这里会调用LLM进行处理
    // 目前我们模拟这个过程

    // 模拟调用LLM
    auto result = callLLM(session);

    // 模拟一些基本的错误重试逻辑
    int attempts = 0;
    const int max_attempts = session->getConfig().max_retries;

    while (result == AgentLoopResult::CONTINUE && attempts < max_attempts) {
      // 检查是否需要重试（这里简化处理）
      if (shouldRetry()) {
        attempts++;
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(session->getConfig().retry_delay.count() * std::pow(session->getConfig().retry_backoff_factor, attempts - 1))));
        result = callLLM(session);
      } else {
        break;
      }
    }

    return result;
  }

  bool handlePendingSubtasks(const Session::Ptr& session) {
    auto subagent_mgr = SubagentManager::getInstance();
    if (subagent_mgr->hasPendingTasks()) {
      subagent_mgr->processPendingTasks(session);
      return true;
    }
    return false;
  }

  bool handlePendingCompactions(const Session::Ptr& session) {
    // 在实际实现中，这里会处理挂起的压缩任务
    // 目前我们简化处理
    return false;
  }

  bool checkContextOverflow(const Session::Ptr& session) {
    size_t token_count = session->getTokenCount();
    return token_count > session->getConfig().compaction_threshold;
  }

  void triggerCompaction(const Session::Ptr& session) {
    // 触发会话压缩
    session->compact(CompactionType::CONTEXT_OVERFLOW);
  }

  AgentLoopResult callLLM(const Session::Ptr& session) {
    // 这里应该实际调用LLM服务
    // 为了演示目的，我们简单地模拟这个过程

    // 获取当前消息历史
    auto messages = session->getLastMessages(10);  // 获取最后10条消息

    // 检查是否有工具调用
    for (const auto& msg : messages) {
      if (msg->getType() == MessageType::ASSISTANT) {
        if (!msg->getToolCalls().empty()) {
          // 执行工具调用
          for (const auto& tool_call : msg->getToolCalls()) {
            executeToolCall(tool_call, session);
          }

          // 添加工具结果到会话
          // 在实际实现中，这里会根据工具执行结果决定下一步
          return AgentLoopResult::CONTINUE;
        }
      }
    }

    // 如果没有工具调用，可能需要继续对话或结束
    // 这里我们简单地返回继续
    return AgentLoopResult::CONTINUE;
  }

  void executeToolCall(const ToolCall& tool_call, const Session::Ptr& session) {
    // 获取工具注册表
    auto tool_registry = ToolRegistry::getInstance();
    auto tool = tool_registry->getToolById(tool_call.name);

    if (!tool) {
      std::cerr << "Tool not found: " << tool_call.name << std::endl;
      return;
    }

    // 创建工具执行上下文
    ToolContext context;
    context.session_id = session->getId();
    context.agent_id = session->getConfig().id;  // 使用会话配置中的ID
    context.session = session;

    // 执行工具
    auto result = tool->execute(tool_call.arguments, context);

    // 创建工具结果消息
    Message::Ptr result_msg = std::make_shared<Message>(MessageType::TOOL_RESULT, "");
    ToolResult tool_result;
    tool_result.call_id = tool_call.id;
    tool_result.tool_name = tool_call.name;
    tool_result.result = result.output;
    tool_result.success = result.success;
    if (result.error.has_value()) {
      tool_result.error = result.error.value();
    }
    result_msg->addToolResult(tool_result);

    // 添加到会话
    session->addMessage(result_msg);
  }

  bool shouldRetry() {
    // 简单的重试判断逻辑
    // 在实际实现中，这里会根据具体的错误类型来判断
    return false;
  }

  Agent::Ptr agent_;
  std::atomic<bool> running_;
  std::mutex mutex_;
};

// AgentLoop implementation
AgentLoop::AgentLoop(const Agent::Ptr& agent) : pimpl_(std::make_unique<Impl>(agent)) {}

AgentLoop::~AgentLoop() = default;

void AgentLoop::run(const std::string& session_id) {
  pimpl_->run(session_id);
}

void AgentLoop::stop() {
  pimpl_->stop();
}

bool AgentLoop::isRunning() const {
  return pimpl_->isRunning();
}

// AgentLoop的公共方法实现现在通过pimpl调用Impl中的公有方法
// 这些方法在实际实现中可能是私有的或受保护的，这里为了编译暂时保留

}  // namespace openagent