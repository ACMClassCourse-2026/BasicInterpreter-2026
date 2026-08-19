# ExecutionContext

## 概述

`ExecutionContext` 聚合一次执行所需的运行环境、输入流和输出流。语句通过该对象访问运行状态和 I/O，不直接使用全局标准流。该类只借用这些对象，不负责释放。

## 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `environment_` | `Environment&` | 借用的程序运行环境 |
| `input_` | `std::istream&` | 借用的输入流 |
| `output_` | `std::ostream&` | 借用的输出流 |

## 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `ExecutionContext` | 绑定运行环境、输入流和输出流 |
| `environment` | 返回运行环境引用；提供可修改和只读两个版本 |
| `input` | 返回输入流引用 |
| `output` | 返回输出流引用 |
