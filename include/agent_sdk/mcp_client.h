#ifndef AGENT_SDK_MCP_CLIENT_H
#define AGENT_SDK_MCP_CLIENT_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <optional>
#include "tool.h"

namespace openagent {

/**
 * @brief MCP服务器类型
 */
enum class MCPServerType {
    REMOTE,     ///< 远程服务器
    LOCAL       ///< 本地服务器
};

/**
 * @brief MCP传输协议
 */
enum class MCPTransportProtocol {
    STDIO,      ///< 标准输入输出
    HTTP_STREAMABLE,  ///< Streamable HTTP
    SSE         ///< Server-Sent Events
};

/**
 * @brief MCP服务器配置
 */
struct MCPServerConfig {
    std::string name;                    ///< 服务器名称
    MCPServerType type;                  ///< 服务器类型
    std::string endpoint;                ///< 端点地址
    MCPTransportProtocol protocol;       ///< 传输协议
    std::map<std::string, std::string> headers;  ///< HTTP头部
    std::optional<std::string> auth_token;       ///< 认证令牌
    std::chrono::milliseconds timeout{5000};     ///< 超时时间
};

/**
 * @brief MCP客户端状态
 */
enum class MCPClientStatus {
    DISCONNECTED,   ///< 未连接
    CONNECTING,     ///< 连接中
    CONNECTED,      ///< 已连接
    FAILED,         ///< 连接失败
    NEEDS_AUTH      ///< 需要认证
};

/**
 * @brief MCP客户端类 - 实现Model Context Protocol客户端
 */
class MCPClient {
public:
    using Ptr = std::shared_ptr<MCPClient>;
    using ConstPtr = std::shared_ptr<const MCPClient>;
    
    /**
     * @brief 构造函数
     */
    MCPClient();
    
    /**
     * @brief 析构函数
     */
    ~MCPClient();

    /**
     * @brief 添加MCP服务器配置
     * @param config 服务器配置
     */
    void addServer(const MCPServerConfig& config);

    /**
     * @brief 连接到所有配置的服务器
     * @return 连接成功的服务器数量
     */
    size_t connect();

    /**
     * @brief 断开所有连接
     */
    void disconnect();

    /**
     * @brief 获取客户端状态
     * @return 客户端状态
     */
    MCPClientStatus getStatus() const;

    /**
     * @brief 获取所有可用工具
     * @return 工具列表
     */
    std::vector<Tool::Ptr> getAvailableTools() const;

    /**
     * @brief 列出指定服务器的工具
     * @param server_name 服务器名称
     * @return 工具ID列表
     */
    std::vector<std::string> listTools(const std::string& server_name) const;

    /**
     * @brief 获取工具定义
     * @param tool_id 工具ID
     * @return 工具定义，如果不存在则返回nullptr
     */
    Tool::Ptr getTool(const std::string& tool_id) const;

    /**
     * @brief 执行MCP工具
     * @param tool_id 工具ID
     * @param arguments 工具参数
     * @return 执行结果
     */
    ToolExecutionResult executeTool(const std::string& tool_id, const std::string& arguments) const;

    /**
     * @brief 设置连接状态变化回调
     * @param callback 回调函数
     */
    void setConnectionCallback(std::function<void(const std::string&, MCPClientStatus)> callback);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace openagent

#endif // AGENT_SDK_MCP_CLIENT_H