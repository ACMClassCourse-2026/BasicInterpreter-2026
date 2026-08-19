# ScopeGuard

## 概述

`ScopeGuard` 使用对象生命周期管理局部作用域：构造时进入新作用域，析构时自动退出。它只借用 `Environment` 且不可复制，可确保正常返回、异常、END 和 EXIT FOR 等路径都能正确清理局部变量。

## 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `environment_` | `Environment&` | 借用的运行环境 |

## 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `ScopeGuard` | 保存环境引用并调用 `push_scope` 创建局部作用域 |
| `~ScopeGuard` | 调用 `pop_scope` 移除构造时创建的作用域 |
