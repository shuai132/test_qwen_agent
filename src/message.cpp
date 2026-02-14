#include "agent_sdk/message.h"

#include <sstream>

namespace openagent {

Message::Message(MessageType type, const std::string& content) : type_(type), timestamp_(std::chrono::system_clock::now()) {
  MessageContent msg_content;
  msg_content.text = content;
  msg_content.type = "text";
  contents_.push_back(msg_content);
  id_ = "msg_" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

Message::Message(MessageType type, const std::vector<MessageContent>& contents)
    : type_(type), contents_(contents), timestamp_(std::chrono::system_clock::now()) {
  id_ = "msg_" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

void Message::setTextContent(const std::string& text) {
  // 清除现有的内容并设置新的文本内容
  contents_.clear();
  MessageContent content;
  content.text = text;
  content.type = "text";
  contents_.push_back(content);
}

}  // namespace openagent