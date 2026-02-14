#ifndef AGENT_SDK_SESSION_H
#define AGENT_SDK_SESSION_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include "message.h"
#include "config.h"

namespace openagent {

/**
 * @brief 会话状态
 */
enum class SessionState {
    IDLE,           ///< 空闲
    RUNNING,        ///< 运行中
    PAUSED,         ///< 暂停
    COMPLETED,      ///< 已完成
    ERROR           ///< 错误
};

/**
 * @brief 会话压缩类型
 */
enum class CompactionType {
    CONTEXT_OVERFLOW,   ///< 上下文溢出
    MANUAL,             ///< 手动触发
    SCHEDULED           ///< 定时压缩
};

/**
 * @brief 会话压缩信息
 */
struct CompactionInfo {
    std::string id;                                    ///< 压缩ID
    CompactionType type;                               ///< 压缩类型
    std::chrono::system_clock::time_point timestamp;   ///< 时间戳
    std::string summary;                               ///< 压缩摘要
    size_t original_size;                             ///< 原始大小(tokens)
    size_t compressed_size;                           ///< 压缩后大小(tokens)
};

/**
 * @brief 会话类 - 管理对话会话
 */
class Session {
public:
    using Ptr = std::shared_ptr<Session>;
    using ConstPtr = std::shared_ptr<const Session>;

    /**
     * @brief 构造函数
     * @param id 会话ID
     * @param config 会话配置
     */
    Session(const std::string& id, const Config& config);

    /**
     * @brief 析构函数
     */
    ~Session();

    /**
     * @brief 添加消息到会话
     * @param message 消息
     */
    void addMessage(const Message::Ptr& message);

    /**
     * @brief 获取会话消息历史
     * @return 消息列表
     */
    std::vector<Message::Ptr> getMessages() const;

    /**
     * @brief 获取最近的消息
     * @param count 获取的消息数量
     * @return 消息列表
     */
    std::vector<Message::Ptr> getLastMessages(size_t count) const;

    /**
     * @brief 获取会话ID
     * @return 会话ID
     */
    const std::string& getId() const;

    /**
     * @brief 获取会话状态
     * @return 会话状态
     */
    SessionState getState() const;

    /**
     * @brief 设置会话状态
     * @param state 新状态
     */
    void setState(SessionState state);

    /**
     * @brief 获取配置
     * @return 会话配置
     */
    const Config& getConfig() const;

    /**
     * @brief 检查是否上下文溢出
     * @return 是否溢出
     */
    bool isContextOverflow() const;

    /**
     * @brief 执行会话压缩
     * @param type 压缩类型
     * @return 压缩信息
     */
    CompactionInfo compact(CompactionType type);

    /**
     * @brief 获取压缩历史
     * @return 压缩历史列表
     */
    std::vector<CompactionInfo> getCompactionHistory() const;

    /**
     * @brief 裁剪旧的工具输出
     */
    void prune();

    /**
     * @brief 获取当前token使用量估算
     * @return token数量
     */
    size_t getTokenCount() const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace openagent

#endif // AGENT_SDK_SESSION_H