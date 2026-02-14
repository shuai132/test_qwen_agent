#ifndef AGENT_SDK_CONTEXT_MANAGER_H
#define AGENT_SDK_CONTEXT_MANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "message.h"
#include "session.h"

namespace openagent {

/**
 * @brief 上下文管理器 - 负责高级上下文操作
 */
class ContextManager {
public:
    using Ptr = std::shared_ptr<ContextManager>;
    
    static ContextManager::Ptr getInstance();
    
    /**
     * @brief 过滤已压缩的消息
     * @param messages 原始消息列表
     * @return 过滤后的消息列表
     */
    std::vector<Message::Ptr> filterCompacted(const std::vector<Message::Ptr>& messages) const;

    /**
     * @brief 检查是否上下文溢出
     * @param session 会话实例
     * @return 是否溢出
     */
    bool isOverflow(const Session::Ptr& session) const;

    /**
     * @brief 创建压缩标记消息
     * @param session 会话实例
     * @param type 压缩类型
     * @return 压缩信息
     */
    CompactionInfo createCompaction(const Session::Ptr& session, CompactionType type);

    /**
     * @brief 处理压缩
     * @param session 会话实例
     * @return 处理结果
     */
    bool processCompaction(const Session::Ptr& session);

private:
    ContextManager();
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace openagent

#endif // AGENT_SDK_CONTEXT_MANAGER_H