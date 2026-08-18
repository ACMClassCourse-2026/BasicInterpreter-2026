# Interpreter

Interpreter 负责将 ParsedInput 分派给 Program，并处理 RUN、LIST、CLEAR、
QUIT 和 HELP。`process_line` 返回 false 表示结束命令循环；运行时错误只终止
当前命令，不销毁解释器。
