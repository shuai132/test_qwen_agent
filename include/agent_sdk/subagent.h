#ifndef AGENT_SDK_SUBAGENT_H
#define AGENT_SDK_SUBAGENT_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "agent.h"
#include "session.h"
#include "message.h"

namespace openagent {

/**
 * @brief 子代理任务类型
 */
enum class SubagentTaskType {
    SUBTASK,        ///< 子任务
    COMPACTION      ///< 压缩任务
};

/**
 * @brief 子代理任务状态
 */
enum class SubagentTaskStatus {
    PENDING,        ///< 待处理
    RUNNING,        ///< 运行中
    COMPLETED,      ///< 已完成
    FAILED          ///< 失败
};

/**
 * @brief 子代理任务
 */
struct SubagentTask {
    std::string id;                      ///< 任务ID
    SubagentTaskType type;               ///< 任务类型
    std::string agent_name;              ///< 子代理名称
    std::string description;             ///< 任务描述
    std::string arguments;               ///< 任务参数
    SubagentTaskStatus status;           ///< 任务状态
    std::chrono::system_clock::time_point created_at;  ///< 创建时间
    std::chrono::system_clock::time_point completed_at;///< 完成时间
    std::string result;                  ///< 任务结果
};

/**
 * @brief 子代理管理器
 */
class SubagentManager {
public:
    using Ptr = std::shared_ptr<SubagentManager>;
    
    static SubagentManager::Ptr getInstance();

    /**
     * @brief 创建子代理会话
     * @param parent_session 父会话
     * @param agent_config 子代理配置
     * @param task_description 任务描述
     * @return 子会话实例
     */
    Session::Ptr createSubagentSession(
        const Session::Ptr& parent_session,
        const Config& agent_config,
        const std::string& task_description
    );

    /**
     * @brief 添加待处理任务
     * @param task 任务
     */
    void addPendingTask(const SubagentTask& task);

    /**
     * @brief 获取待处理任务
     * @return 任务列表
     */
    std::vector<SubagentTask> getPendingTasks() const;

    /**
     * @brief 处理待处理任务
     * @param parent_session 父会话
     */
    void processPendingTasks(const Session::Ptr& parent_session);

    /**
     * @brief 检查是否有待处理任务
     * @return 是否有待处理任务
     */
    bool hasPendingTasks() const;

private:
    SubagentManager();
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace openagent

#endif // AGENT_SDK_SUBAGENT_H