# Environment

Environment 使用作用域栈保存 VariableSlot。读取和赋值从内向外查找，声明只
检查当前作用域。VariableSlot 按值保存标量或 ArrayValue。