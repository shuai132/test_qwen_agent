#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "agent_sdk/agent.h"
#include "agent_sdk/config.h"
#include "agent_sdk/context_manager.h"
#include "agent_sdk/mcp_client.h"
#include "agent_sdk/message.h"
#include "agent_sdk/session.h"
#include "agent_sdk/subagent.h"
#include "agent_sdk/tool.h"

using namespace openagent;

// 测试配置
Config createTestConfig() {
  Config config;
  config.id = "test_agent";
  config.name = "Test Agent";
  config.description = "Test Agent for Unit Testing";
  config.model = "test-model";
  config.api_key = "test-key";
  config.base_url = "https://test-api.example.com";
  config.max_context_tokens = 10000;
  config.compaction_threshold = 8000;
  config.prune_protected_tokens = 4000;
  config.prune_minimum_tokens = 2000;
  return config;
}

// 测试消息类
TEST(MessageTest, BasicMessageCreation) {
  Message msg(MessageType::USER, "Hello, world!");

  EXPECT_EQ(msg.getType(), MessageType::USER);
  ASSERT_EQ(msg.getContents().size(), 1);
  EXPECT_EQ(msg.getContents()[0].text, "Hello, world!");
  EXPECT_EQ(msg.getContents()[0].type, "text");
}

TEST(MessageTest, AddContent) {
  Message msg(MessageType::ASSISTANT, "");  // 这会创建一个空内容

  MessageContent content;
  content.text = "Additional content";
  content.type = "text";
  msg.addContent(content);

  ASSERT_EQ(msg.getContents().size(), 2);                      // 原始空内容 + 新增内容 = 2
  EXPECT_EQ(msg.getContents()[1].text, "Additional content");  // 新内容在索引1处
}

TEST(MessageTest, ToolCallAndResult) {
  Message msg(MessageType::ASSISTANT, "Processing...");

  ToolCall call;
  call.id = "call_123";
  call.name = "test_tool";
  call.arguments = R"({"param": "value"})";
  msg.addToolCall(call);

  ASSERT_EQ(msg.getToolCalls().size(), 1);
  EXPECT_EQ(msg.getToolCalls()[0].id, "call_123");

  ToolResult result;
  result.call_id = "call_123";
  result.tool_name = "test_tool";
  result.result = "Success";
  result.success = true;
  msg.addToolResult(result);

  ASSERT_EQ(msg.getToolResults().size(), 1);
  EXPECT_EQ(msg.getToolResults()[0].call_id, "call_123");
  EXPECT_TRUE(msg.getToolResults()[0].success);
}

// 测试工具类
TEST(ToolTest, ToolCreationAndExecution) {
  Tool::Definition def;
  def.id = "test_tool";
  def.description = "A test tool";
  def.parameters_schema = R"({"type": "object", "properties": {"input": {"type": "string"}}})";
  def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
    ToolExecutionResult result;
    result.title = "Test Tool Result";
    result.output = "Executed with args: " + args;
    result.success = true;
    return result;
  };

  Tool tool(def);

  EXPECT_EQ(tool.getId(), "test_tool");
  EXPECT_EQ(tool.getDescription(), "A test tool");

  ToolContext ctx;
  ctx.session_id = "test_session";

  auto result = tool.execute(R"({"input": "hello"})", ctx);
  EXPECT_EQ(result.title, "Test Tool Result");
  EXPECT_EQ(result.output, "Executed with args: {\"input\": \"hello\"}");
  EXPECT_TRUE(result.success);
}

TEST(ToolRegistryTest, RegisterAndGetTool) {
  auto registry = ToolRegistry::getInstance();

  Tool::Definition def;
  def.id = "registry_test_tool";
  def.description = "A tool for registry testing";
  def.parameters_schema = "{}";
  def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
    ToolExecutionResult result;
    result.title = "Registry Test Result";
    result.output = "Registry test executed";
    result.success = true;
    return result;
  };

  auto tool = std::make_shared<Tool>(def);
  registry->registerTool(tool);

  auto retrieved_tool = registry->getToolById("registry_test_tool");
  ASSERT_NE(retrieved_tool, nullptr);
  EXPECT_EQ(retrieved_tool->getId(), "registry_test_tool");

  auto all_tools = registry->getAllTools();
  EXPECT_FALSE(all_tools.empty());
}

// 测试会话类
TEST(SessionTest, SessionCreationAndMessages) {
  Config config = createTestConfig();
  Session session("test_session_123", config);

  EXPECT_EQ(session.getId(), "test_session_123");
  EXPECT_EQ(session.getState(), SessionState::IDLE);
  EXPECT_EQ(session.getConfig().id, "test_agent");

  Message::Ptr msg = std::make_shared<Message>(MessageType::USER, "Test message");
  session.addMessage(msg);

  auto messages = session.getMessages();
  ASSERT_EQ(messages.size(), 1);
  EXPECT_EQ(messages[0]->getContents()[0].text, "Test message");

  auto last_messages = session.getLastMessages(5);
  ASSERT_EQ(last_messages.size(), 1);
}

TEST(SessionTest, TokenCountEstimation) {
  Config config = createTestConfig();
  Session session("test_session_tokens", config);

  Message::Ptr msg1 = std::make_shared<Message>(MessageType::USER, "Short message");
  session.addMessage(msg1);

  Message::Ptr msg2 = std::make_shared<Message>(MessageType::ASSISTANT, std::string(1000, 'a'));  // 1000 chars
  session.addMessage(msg2);

  size_t token_count = session.getTokenCount();
  // Should be roughly (11 + 1000) / 4 = ~252 (plus overhead)
  EXPECT_GT(token_count, 0);
}

TEST(SessionTest, Compaction) {
  Config config = createTestConfig();
  Session session("test_session_compact", config);

  // Add several messages to trigger compaction
  for (int i = 0; i < 10; ++i) {
    Message::Ptr msg = std::make_shared<Message>(MessageType::USER, "Message " + std::to_string(i));
    session.addMessage(msg);
  }

  auto initial_count = session.getMessages().size();
  EXPECT_EQ(initial_count, 10);

  // Perform compaction
  auto compaction_info = session.compact(CompactionType::MANUAL);

  auto final_count = session.getMessages().size();
  // After compaction, should have fewer messages (implementation keeps last 5)
  EXPECT_LE(final_count, initial_count);
  EXPECT_GE(final_count, 1);  // Should still have at least some messages

  // Check compaction history
  auto history = session.getCompactionHistory();
  ASSERT_EQ(history.size(), 1);
  EXPECT_EQ(history[0].type, CompactionType::MANUAL);
}

TEST(SessionTest, PruneFunctionality) {
  Config config = createTestConfig();
  config.prune_protected_tokens = 100;  // Lower threshold for testing
  config.prune_minimum_tokens = 50;

  Session session("test_session_prune", config);

  // Add messages that exceed the pruning threshold
  for (int i = 0; i < 50; ++i) {
    Message::Ptr msg = std::make_shared<Message>(MessageType::TOOL_RESULT, std::string(50, 'a'));  // 50 chars each
    session.addMessage(msg);
  }

  size_t before_prune = session.getTokenCount();
  session.prune();
  size_t after_prune = session.getTokenCount();

  // After pruning, token count should be reduced
  EXPECT_LT(after_prune, before_prune);
}

// 测试MCP客户端
TEST(MCPClientTest, ClientInitialization) {
  MCPClient client;

  EXPECT_EQ(client.getStatus(), MCPClientStatus::DISCONNECTED);

  MCPServerConfig config;
  config.name = "test_server";
  config.type = MCPServerType::REMOTE;
  config.endpoint = "https://test-mcp.example.com";
  config.protocol = MCPTransportProtocol::HTTP_STREAMABLE;

  client.addServer(config);

  // Connect to server
  size_t connected = client.connect();
  EXPECT_GT(connected, 0);  // Should connect successfully in our mock implementation

  EXPECT_EQ(client.getStatus(), MCPClientStatus::CONNECTED);

  // Get available tools
  auto tools = client.getAvailableTools();
  EXPECT_FALSE(tools.empty());

  // Test tool execution
  if (!tools.empty()) {
    auto tool_id = tools[0]->getId();
    auto result = client.executeTool(tool_id, R"({"input": "test"})");
    EXPECT_TRUE(result.success);
  }

  client.disconnect();
  EXPECT_EQ(client.getStatus(), MCPClientStatus::DISCONNECTED);
}

// 测试子代理管理器
TEST(SubagentManagerTest, CreateAndProcessSubagent) {
  auto manager = SubagentManager::getInstance();

  // Create a parent session
  Config parent_config = createTestConfig();
  auto parent_session = std::make_shared<Session>("parent_session", parent_config);

  // Create sub-agent config
  Config sub_config = createTestConfig();
  sub_config.id = "subagent_test";

  // Create sub-session
  auto sub_session = manager->createSubagentSession(parent_session, sub_config, "Test sub-task");
  ASSERT_NE(sub_session, nullptr);
  EXPECT_EQ(sub_session->getId(), "parent_session_sub_1");

  // Add a test message to sub-session
  Message::Ptr msg = std::make_shared<Message>(MessageType::USER, "Subagent test message");
  sub_session->addMessage(msg);

  // Verify message was added
  auto sub_messages = sub_session->getMessages();
  ASSERT_EQ(sub_messages.size(), 1);
  EXPECT_EQ(sub_messages[0]->getContents()[0].text, "Subagent test message");
}

// 测试上下文管理器
TEST(ContextManagerTest, OverflowDetection) {
  auto manager = ContextManager::getInstance();

  Config config = createTestConfig();
  config.max_context_tokens = 100;  // Low threshold for testing

  auto session = std::make_shared<Session>("test_overflow_session", config);

  // Add messages to exceed the threshold
  for (int i = 0; i < 30; ++i) {
    Message::Ptr msg = std::make_shared<Message>(MessageType::USER, std::string(10, 'a'));  // 10 chars each
    session->addMessage(msg);
  }

  bool overflow = manager->isOverflow(session);
  EXPECT_TRUE(overflow);
}

TEST(ContextManagerTest, CompactionProcess) {
  auto manager = ContextManager::getInstance();

  Config config = createTestConfig();
  auto session = std::make_shared<Session>("test_compact_session", config);

  // Add some messages
  for (int i = 0; i < 5; ++i) {
    Message::Ptr msg = std::make_shared<Message>(MessageType::USER, "Message " + std::to_string(i));
    session->addMessage(msg);
  }

  size_t before_tokens = session->getTokenCount();

  // Process compaction
  bool success = manager->processCompaction(session);
  EXPECT_TRUE(success);

  // Should have added a continuation message
  auto messages = session->getMessages();
  EXPECT_GT(messages.size(), 5);  // Original 5 plus continuation message

  // Check that a compaction was recorded
  auto history = session->getCompactionHistory();
  EXPECT_FALSE(history.empty());
}

// 测试Agent基本功能
TEST(AgentTest, AgentInitialization) {
  Config config = createTestConfig();
  Agent agent(config);

  // Add a test tool
  Tool::Definition def;
  def.id = "agent_test_tool";
  def.description = "Tool for agent testing";
  def.parameters_schema = "{}";
  def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
    ToolExecutionResult result;
    result.title = "Agent Test Tool Result";
    result.output = "Agent test tool executed";
    result.success = true;
    return result;
  };

  auto tool = std::make_shared<Tool>(def);
  agent.addTool(tool);

  // Verify tool was registered
  auto registry = ToolRegistry::getInstance();
  auto registered_tool = registry->getToolById("agent_test_tool");
  ASSERT_NE(registered_tool, nullptr);
}

// 集成测试：完整的Agent运行流程
TEST(IntegrationTest, CompleteAgentFlow) {
  Config config = createTestConfig();
  config.mcp_servers = {"https://test-mcp.example.com"};  // Enable MCP

  Agent agent(config);

  // Add a simple test tool
  Tool::Definition def;
  def.id = "integration_test_tool";
  def.description = "Tool for integration testing";
  def.parameters_schema = R"({"type": "object", "properties": {"value": {"type": "string"}}})";
  def.execute_func = [](const std::string& args, const ToolContext& ctx) -> ToolExecutionResult {
    ToolExecutionResult result;
    result.title = "Integration Test Result";
    result.output = "Integration test executed with: " + args;
    result.success = true;
    return result;
  };

  auto tool = std::make_shared<Tool>(def);
  agent.addTool(tool);

  // Create a session and add an initial message
  auto session = agent.getSession();
  ASSERT_NE(session, nullptr);

  Message::Ptr initial_msg = std::make_shared<Message>(MessageType::USER, "Start integration test");
  session->addMessage(initial_msg);

  // Verify session was created properly
  EXPECT_EQ(session->getId(), "");  // Will be set when agent runs
  EXPECT_EQ(session->getState(), SessionState::IDLE);

  // Note: We're not actually running the agent loop here as it would be a long-running operation
  // Instead, we're testing that the setup is correct
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}