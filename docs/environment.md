# Environment

## 概述

`Environment` 使用作用域栈保存变量。读取和赋值从内层作用域向外查找，声明只检查当前作用域，因此内层变量可以遮蔽同名外层变量。

## `VariableSlot` 结构体

表示一个变量槽，按值保存标量或数组的类型和数据。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `type` | `BasicType` | 变量的声明类型，默认为 `Single` |
| `is_array` | `bool` | 标记该变量是否为数组 |
| `scalar` | `Value` | 标量变量的值；数组变量不使用该成员 |
| `array` | `std::optional<ArrayValue>` | 数组数据；标量变量不使用该成员 |

## `Environment` 类

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `scopes_` | `std::vector<std::unordered_map<std::string, VariableSlot>>` | 作用域栈；每层保存变量名到变量槽的映射 |

### 私有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `find_slot` | 从内层到外层查找变量槽；提供可修改和只读两个版本，找不到时产生未定义变量错误 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `Environment` | 创建只含全局作用域的运行环境 |
| `push_scope` | 在栈顶创建一个新的局部作用域 |
| `pop_scope` | 移除最内层局部作用域，不允许移除全局作用域 |
| `clear` | 清空所有变量并重新创建空的全局作用域 |
| `scope_depth` | 返回当前作用域层数 |
| `declare_scalar` | 在当前作用域声明标量，并将初值转换为声明类型 |
| `declare_fixed_array` | 在当前作用域声明指定维度的定长数组 |
| `declare_dynamic_array` | 在当前作用域声明尚未分配长度的动态数组 |
| `assign` | 查找标量变量，并将给定值转换为变量类型后赋值 |
| `assign_element` | 查找数组变量，根据下标赋值并执行元素类型转换 |
| `read` | 查找并返回标量值的只读引用 |
| `read_element` | 查找数组变量并返回指定元素的只读引用 |
| `redim` | 重新分配动态数组长度，可选择保留已有元素 |
| `erase_array` | 释放动态数组已分配的空间，使其恢复为未分配状态 |
