# Expression

表达式节点均由父节点通过裸指针拥有。Literal、Variable、Unary、Binary 和
Conversion 节点分别处理字面量、变量或数组访问、一元运算、算术运算和显式
转换。`evaluate` 只借用 Environment，不修改运行状态。
