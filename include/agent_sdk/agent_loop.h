#ifndef AGENT_SDK_AGENT_LOOP_H
#define AGENT_SDK_AGENT_LOOP_H

#include <memory>
#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <condition_variable>
#include "agent.h"
#include "session.h"
#include "message.h"
#include "tool.h"

namespace openagent {

/**
 * @brief Agent循环结果
 */
enum class AgentLoopResult {
    CONTINUE,       ///< 继续循环
    STOP,           ///< 停止循环
    COMPACT         ///< 需要压缩
};

/**
 * @brief Agent循环处理器
 */
class AgentLoop {
public:
    using Ptr = std::shared_ptr<AgentLoop>;
    
    /**
     * @brief 构造函数
     * @param agent Agent实例
     */
    explicit AgentLoop(const Agent::Ptr& agent);

    /**
     * @brief 析构函数
     */
    ~AgentLoop();

    /**
     * @brief 运行Agent主循环
     * @param session_id 会话ID
     */
    void run(const std::string& session_id);

    /**
     * @brief 停止循环
     */
    void stop();

    /**
     * @brief 检查循环是否正在运行
     * @return 是否正在运行
     */
    bool isRunning() const;

private:
    /**
     * @brief 主循环实现
     * @param session_id 会话ID
     */
    void loop(const std::string& session_id);

    /**
     * @brief 处理单步执行
     * @param session 会话实例
     * @return 循环结果
     */
    AgentLoopResult processStep(const Session::Ptr& session);

    /**
     * @brief 处理挂起的子任务
     * @param session 会话实例
     * @return 是否处理了子任务
     */
    bool handlePendingSubtasks(const Session::Ptr& session);

    /**
     * @brief 处理挂起的压缩任务
     * @param session 会话实例
     * @return 是否处理了压缩任务
     */
    bool handlePendingCompactions(const Session::Ptr& session);

    /**
     * @brief 检查上下文溢出
     * @param session 会话实例
     * @return 是否溢出
     */
    bool checkContextOverflow(const Session::Ptr& session);

    /**
     * @brief 触发会话压缩
     * @param session 会话实例
     */
    void triggerCompaction(const Session::Ptr& session);

    /**
     * @brief 调用LLM处理消息
     * @param session 会话实例
     * @return 循环结果
     */
    AgentLoopResult callLLM(const Session::Ptr& session);

    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace openagent

#endif // AGENT_SDK_AGENT_LOOP_H