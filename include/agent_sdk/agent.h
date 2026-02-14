#ifndef AGENT_SDK_AGENT_H
#define AGENT_SDK_AGENT_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <chrono>
#include "message.h"
#include "tool.h"
#include "session.h"
#include "config.h"

namespace openagent {

/**
 * @brief Agent类 - 核心智能体实现
 */
class Agent : public std::enable_shared_from_this<Agent> {
public:
    using Ptr = std::shared_ptr<Agent>;
    using ConstPtr = std::shared_ptr<const Agent>;
    
    /**
     * @brief 构造函数
     * @param config Agent配置
     */
    explicit Agent(const Config& config);
    
    /**
     * @brief 析构函数
     */
    ~Agent();

    /**
     * @brief 启动Agent主循环
     * @param sessionId 会话ID
     */
    void run(const std::string& sessionId);

    /**
     * @brief 停止Agent
     */
    void stop();

    /**
     * @brief 添加工具到Agent
     * @param tool 工具实例
     */
    void addTool(const Tool::Ptr& tool);

    /**
     * @brief 获取当前会话
     * @return 当前会话实例
     */
    Session::Ptr getSession();

    /**
     * @brief 设置消息处理器
     * @param handler 消息处理回调
     */
    void setMessageHandler(std::function<void(const Message&)> handler);

    /**
     * @brief 获取Agent ID
     * @return Agent ID
     */
    const std::string& getId() const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace openagent

#endif // AGENT_SDK_AGENT_H