# ExecutionContext

ExecutionContext 聚合一次执行所需的 Environment、输入流和输出流引用。它不
拥有这些对象。所有语句通过该对象访问状态和 I/O，不直接使用全局标准流。
