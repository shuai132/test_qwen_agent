#include "agent_sdk/agent.h"

#include <algorithm>
#include <iostream>

#include "agent_sdk/agent_loop.h"
#include "agent_sdk/mcp_client.h"
#include "agent_sdk/session.h"
#include "agent_sdk/tool.h"

namespace openagent {

class Agent::Impl {
 public:
  explicit Impl(const Config& config) : config_(config), running_(false), session_(nullptr) {}

  ~Impl() {
    stop();
  }

  void run(const std::string& session_id, const std::shared_ptr<Agent>& agent) {
    if (running_) {
      return;
    }

    running_ = true;

    // 创建会话
    session_ = std::make_shared<Session>(session_id, config_);

    // 初始化MCP客户端
    initializeMCP();

    // 创建并启动Agent循环
    agent_loop_ = std::make_shared<AgentLoop>(agent);
    agent_loop_->run(session_id);
  }

  void stop() {
    if (!running_) {
      return;
    }

    running_ = false;

    if (agent_loop_) {
      agent_loop_->stop();
    }

    // 断开MCP连接
    if (mcp_client_) {
      mcp_client_->disconnect();
    }
  }

  void addTool(const Tool::Ptr& tool) {
    tools_.push_back(tool);
    ToolRegistry::getInstance()->registerTool(tool);
  }

  Session::Ptr getSession() {
    return session_;
  }

  void setMessageHandler(std::function<void(const Message&)> handler) {
    message_handler_ = handler;
  }

  const std::string& getId() const {
    return config_.id;
  }

  void initializeMCP() {
    if (!config_.mcp_servers.empty()) {
      mcp_client_ = std::make_shared<MCPClient>();

      for (const auto& server_url : config_.mcp_servers) {
        MCPServerConfig server_config;
        server_config.name = "mcp_server_" + std::to_string(mcp_servers_.size());
        server_config.type = MCPServerType::REMOTE;
        server_config.endpoint = server_url;
        server_config.protocol = MCPTransportProtocol::HTTP_STREAMABLE;
        server_config.timeout = config_.mcp_connect_timeout;

        mcp_client_->addServer(server_config);
      }

      mcp_client_->connect();
    }
  }

  Config config_;
  std::atomic<bool> running_;
  Session::Ptr session_;
  std::shared_ptr<AgentLoop> agent_loop_;
  std::vector<Tool::Ptr> tools_;
  std::function<void(const Message&)> message_handler_;
  std::shared_ptr<MCPClient> mcp_client_;
  std::vector<MCPServerConfig> mcp_servers_;
};

// Agent implementation
Agent::Agent(const Config& config) : pimpl_(std::make_unique<Impl>(config)) {}

Agent::~Agent() = default;

void Agent::run(const std::string& sessionId) {
  pimpl_->run(sessionId, shared_from_this());
}

void Agent::stop() {
  pimpl_->stop();
}

void Agent::addTool(const Tool::Ptr& tool) {
  pimpl_->addTool(tool);
}

Session::Ptr Agent::getSession() {
  return pimpl_->getSession();
}

void Agent::setMessageHandler(std::function<void(const Message&)> handler) {
  pimpl_->setMessageHandler(handler);
}

}  // namespace openagent