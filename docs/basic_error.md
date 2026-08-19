# BasicError

## 概述

`BasicError` 表示 `SYNTAX.md` 规定的稳定语言错误。`ErrorCode` 与错误文本一一对应。

## `ErrorCode` 枚举类

| 值名称 | 含义 |
| :-: | :-: | :-: |
| `Syntax` | 词法、语法或程序块结构错误 |
| `DivideByZero` | 除数为零 |
| `InvalidNumber` | INPUT 得到非法数值 |
| `VariableNotDefined` | 使用了未声明或不可见的变量 |
| `VariableAlreadyDefined` | 在同一作用域重复声明变量 |
| `TypeMismatch` | 操作数或赋值类型不符合要求 |
| `Overflow` | 字面量、运算或类型转换溢出 |
| `InvalidArraySize` | 数组长度非法或容量溢出 |
| `ArrayNotDimensioned` | 动态数组尚未分配 |
| `SubscriptOutOfRange` | 数组下标越界 |
| `InvalidRedim` | 对不允许调整的数组使用 REDIM/ERASE |
| `StepCannotBeZero` | FOR 的步长为零 |

## `BasicError` 错误类

该错误类持有一个 `ErrorCode` 表示错误内容。
