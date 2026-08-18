# ScopeGuard

ScopeGuard 构造时进入新作用域，析构时退出该作用域。它只借用 Environment，
不可复制，用于确保正常返回、异常、END 和 EXIT FOR 路径均正确清理局部状态。
