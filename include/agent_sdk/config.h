#ifndef AGENT_SDK_CONFIG_H
#define AGENT_SDK_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <chrono>

namespace openagent {

/**
 * @brief Agent配置
 */
struct Config {
    std::string id;                                      ///< Agent ID
    std::string name;                                    ///< Agent名称
    std::string description;                             ///< Agent描述
    std::string model;                                   ///< 使用的模型
    std::string api_key;                                 ///< API密钥
    std::string base_url;                                ///< API基础URL
    
    // 会话配置
    size_t max_context_tokens = 100000;                 ///< 最大上下文tokens数
    size_t max_output_tokens = 4096;                     ///< 最大输出tokens数
    size_t max_tool_calls_per_step = 10;                 ///< 每步最大工具调用次数
    std::chrono::milliseconds step_timeout{30000};       ///< 单步超时时间
    std::chrono::milliseconds session_timeout{3600000};  ///< 会话超时时间
    
    // 工具配置
    std::vector<std::string> enabled_tools;              ///< 启用的工具列表
    std::vector<std::string> disabled_tools;             ///< 禁用的工具列表
    std::map<std::string, std::string> tool_configs;     ///< 工具特定配置
    
    // MCP配置
    std::vector<std::string> mcp_servers;                ///< MCP服务器列表
    std::chrono::milliseconds mcp_connect_timeout{5000}; ///< MCP连接超时时间
    
    // 压缩配置
    size_t compaction_threshold = 80000;                 ///< 压缩阈值(tokens)
    size_t prune_protected_tokens = 40000;               ///< 保护的tokens数(裁剪时保留)
    size_t prune_minimum_tokens = 20000;                 ///< 最小裁剪量
    
    // 重试配置
    int max_retries = 3;                                 ///< 最大重试次数
    std::chrono::milliseconds retry_delay{1000};         ///< 重试延迟
    double retry_backoff_factor = 2.0;                   ///< 重试退避因子
    
    // 其他配置
    std::map<std::string, std::string> properties;       ///< 扩展属性
};

} // namespace openagent

#endif // AGENT_SDK_CONFIG_H