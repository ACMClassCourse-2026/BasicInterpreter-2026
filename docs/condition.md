# Condition

Condition 将比较表达式组合为 NOT、AND 和 OR。比较两端必须同类型；AND 与 OR
短路求值。每个组合节点拥有其子条件，析构时递归释放。
