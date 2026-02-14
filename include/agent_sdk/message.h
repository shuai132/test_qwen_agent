#ifndef AGENT_SDK_MESSAGE_H
#define AGENT_SDK_MESSAGE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

namespace openagent {

/**
 * @brief 消息类型枚举
 */
enum class MessageType {
    USER,
    ASSISTANT,
    SYSTEM,
    TOOL_CALL,
    TOOL_RESULT
};

/**
 * @brief 消息内容结构
 */
struct MessageContent {
    std::string text;                    ///< 文本内容
    std::string type;                    ///< 内容类型 (text, image_url等)
    std::map<std::string, std::string> metadata;  ///< 元数据
};

/**
 * @brief 工具调用信息
 */
struct ToolCall {
    std::string id;                      ///< 工具调用ID
    std::string name;                    ///< 工具名称
    std::string arguments;               ///< 工具参数(JSON格式)
};

/**
 * @brief 工具结果信息
 */
struct ToolResult {
    std::string call_id;                 ///< 对应的工具调用ID
    std::string tool_name;              ///< 工具名称
    std::string result;                  ///< 工具执行结果
    bool success;                       ///< 是否成功
    std::string error;                   ///< 错误信息(如果有)
};

/**
 * @brief 消息完成状态
 */
enum class FinishReason {
    STOP,           ///< 正常停止
    LENGTH,         ///< 达到最大长度
    TOOL_CALLS,     ///< 发起工具调用
    CONTENT_FILTER  ///< 内容过滤
};

/**
 * @brief 消息类 - 表示对话中的单条消息
 */
class Message {
public:
    using Ptr = std::shared_ptr<Message>;
    using ConstPtr = std::shared_ptr<const Message>;

    Message() = default;
    Message(MessageType type, const std::string& content);
    Message(MessageType type, const std::vector<MessageContent>& contents);

    MessageType getType() const { return type_; }
    void setType(MessageType type) { type_ = type; }

    const std::vector<MessageContent>& getContents() const { return contents_; }
    void addContent(const MessageContent& content) { contents_.push_back(content); }
    void setTextContent(const std::string& text);

    const std::vector<ToolCall>& getToolCalls() const { return tool_calls_; }
    void addToolCall(const ToolCall& call) { tool_calls_.push_back(call); }

    const std::vector<ToolResult>& getToolResults() const { return tool_results_; }
    void addToolResult(const ToolResult& result) { tool_results_.push_back(result); }

    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    void setTimestamp(std::chrono::system_clock::time_point time) { timestamp_ = time; }

    const std::map<std::string, std::string>& getMetadata() const { return metadata_; }
    void setMetadata(const std::map<std::string, std::string>& meta) { metadata_ = meta; }

    std::optional<FinishReason> getFinishReason() const { return finish_reason_; }
    void setFinishReason(FinishReason reason) { finish_reason_ = reason; }

    const std::string& getId() const { return id_; }
    void setId(const std::string& id) { id_ = id; }

private:
    std::string id_;                                    ///< 消息唯一标识符
    MessageType type_;                                  ///< 消息类型
    std::vector<MessageContent> contents_;              ///< 消息内容列表
    std::vector<ToolCall> tool_calls_;                  ///< 工具调用列表
    std::vector<ToolResult> tool_results_;              ///< 工具结果列表
    std::chrono::system_clock::time_point timestamp_;   ///< 时间戳
    std::map<std::string, std::string> metadata_;       ///< 元数据
    std::optional<FinishReason> finish_reason_;         ///< 完成原因
};

} // namespace openagent

#endif // AGENT_SDK_MESSAGE_H