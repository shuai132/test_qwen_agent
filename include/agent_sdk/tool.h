#ifndef AGENT_SDK_TOOL_H
#define AGENT_SDK_TOOL_H

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <any>
#include <optional>
#include "message.h"

namespace openagent {

class Session;

/**
 * @brief 工具执行上下文
 */
struct ToolContext {
    std::string session_id;              ///< 会话ID
    std::string agent_id;                ///< Agent ID
    std::shared_ptr<Session> session;    ///< 当前会话
    std::map<std::string, std::any> properties;  ///< 扩展属性
};

/**
 * @brief 工具执行结果
 */
struct ToolExecutionResult {
    std::string title;                   ///< 结果标题
    std::map<std::string, std::string> metadata;  ///< 元数据
    std::string output;                  ///< 执行输出
    bool success;                        ///< 是否成功
    std::optional<std::string> error;    ///< 错误信息
};

/**
 * @brief 工具定义接口
 */
class Tool {
public:
    using Ptr = std::shared_ptr<Tool>;
    using ConstPtr = std::shared_ptr<const Tool>;
    using ExecuteFunc = std::function<ToolExecutionResult(const std::string&, const ToolContext&)>;

    struct Definition {
        std::string id;                  ///< 工具ID
        std::string description;         ///< 工具描述
        std::string parameters_schema;   ///< 参数JSON Schema
        ExecuteFunc execute_func;        ///< 执行函数
    };

    /**
     * @brief 构造函数
     * @param definition 工具定义
     */
    explicit Tool(const Definition& definition);

    /**
     * @brief 获取工具ID
     * @return 工具ID
     */
    const std::string& getId() const;

    /**
     * @brief 获取工具描述
     * @return 工具描述
     */
    const std::string& getDescription() const;

    /**
     * @brief 获取参数模式
     * @return 参数JSON Schema
     */
    const std::string& getParametersSchema() const;

    /**
     * @brief 执行工具
     * @param arguments 工具参数(JSON格式)
     * @param context 执行上下文
     * @return 执行结果
     */
    ToolExecutionResult execute(const std::string& arguments, const ToolContext& context) const;

private:
    Definition definition_;
};

/**
 * @brief 工具注册表
 */
class ToolRegistry {
public:
    using Ptr = std::shared_ptr<ToolRegistry>;
    
    static ToolRegistry::Ptr getInstance();

    /**
     * @brief 注册工具
     * @param tool 工具实例
     */
    void registerTool(const Tool::Ptr& tool);

    /**
     * @brief 获取所有工具
     * @return 工具列表
     */
    std::vector<Tool::Ptr> getAllTools() const;

    /**
     * @brief 根据ID获取工具
     * @param id 工具ID
     * @return 工具实例，如果不存在则返回nullptr
     */
    Tool::Ptr getToolById(const std::string& id) const;

private:
    ToolRegistry();
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace openagent

#endif // AGENT_SDK_TOOL_H