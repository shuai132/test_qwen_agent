# OpenCode Built-in Tools

This directory contains the implementation of built-in tools for the OpenCode agent system. These tools provide core functionality that enables the agent to interact with the system, manipulate files, search code, and more.

## Available Built-in Tools

### 1. BashTool (`bash`)
Execute shell commands and return their output.

Parameters:
- `command`: The shell command to execute

### 2. ReadTool (`read`)
Read the content of a file.

Parameters:
- `filepath`: The path to the file to read

### 3. GlobTool (`glob`)
Find files using glob pattern matching.

Parameters:
- `pattern`: The glob pattern to match files

### 4. GrepTool (`grep`)
Search for patterns in files.

Parameters:
- `pattern`: The pattern to search for
- `filepath`: The path to the file(s) to search in

### 5. EditTool (`edit`)
Edit a file by replacing content.

Parameters:
- `filepath`: The path to the file to edit
- `old_content`: The content to replace
- `new_content`: The new content to insert

### 6. WriteTool (`write`)
Write content to a file.

Parameters:
- `filepath`: The path to the file to write
- `content`: The content to write to the file

### 7. QuestionTool (`question`)
Ask a question to the user.

Parameters:
- `question`: The question to ask the user

### 8. TaskTool (`task`)
Execute a sub-task or delegate to another agent.

Parameters:
- `task_description`: Description of the task to execute

### 9. SkillTool (`skill`)
Load and execute a skill on demand.

Parameters:
- `skill_name`: Name of the skill to execute

### 10. WebSearchTool (`web_search`)
Perform a web search for information.

Parameters:
- `query`: The search query

### 11. WebFetchTool (`web_fetch`)
Fetch content from a URL.

Parameters:
- `url`: The URL to fetch content from

### 12. CodeSearchTool (`code_search`)
Search for code patterns in the codebase.

Parameters:
- `query`: The code search query

### 13. ApplyPatchTool (`apply_patch`)
Apply a patch to a file in GPT format.

Parameters:
- `filepath`: The path to the file to patch
- `patch_content`: The patch content in GPT format

### 14. InvalidTool (`invalid`)
Handles invalid tool calls by providing helpful suggestions.

Parameters:
- `original_tool`: The originally called tool that was invalid
- `suggestion`: Suggested alternative tool or correction

## Usage

To register all built-in tools, use the `BuiltInTools::registerAll()` method:

```cpp
auto registry = openagent::ToolRegistry::getInstance();
openagent::BuiltInTools::registerAll(registry);
```

Each tool follows the same interface defined in `tool.h` and can be retrieved from the registry by its ID.

## Security Features

- Path traversal prevention in file operations
- Content size limits to prevent huge outputs
- Proper error handling and validation

## Future Enhancements

- Integration with MCP (Model Context Protocol) for external tools
- Enhanced permission system for fine-grained access control
- Improved pattern matching for glob operations
- Actual web search and fetch implementations