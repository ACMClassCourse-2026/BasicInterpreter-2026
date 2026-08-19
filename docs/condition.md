# Condition

## 概述

Condition 将条件表达式组合为 NOT、AND 和 OR，使用树形结构维护表达式。比较两端必须同类型；AND 与 OR 短路求值。每个组合节点拥有其子条件，析构时递归释放。

## 枚举类

### `ComparisonOperator`

| 值名称 | 含义 |
| :-: | :-: |
| `Equal` | 等于 |
| `NotEqual` | 不等于 |
| `Greater` | 大于 |
| `Less` | 小于 |
| `GreaterEqual` | 大于等于 |
| `LessEqual` | 小于等于 |

### `LogicalOperator`

| 值名称 | 含义 |
| :-: | :-: |
| `And` | 逻辑与 |
| `Or` | 逻辑或 |

## `Condition` 抽象类

条件表达式节点的抽象基类。虚析构函数保证通过基类指针正确释放子类对象。

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `evaluate` | 在给定运行环境中判定条件是否成立 |

## `ComparisonCondition` 类

### 概述

表示单一比较条件，公有继承 `Condition`，作为条件树的叶子节点。该类拥有左右表达式且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `operation_` | `ComparisonOperator` | 比较运算类型 |
| `left_` | `Expression*` | 拥有的左侧表达式 |
| `right_` | `Expression*` | 拥有的右侧表达式 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `ComparisonCondition` | 接收比较运算类型和左右表达式，并取得两个表达式的所有权 |
| `evaluate` | 对两侧表达式求值并完成指定比较；两侧类型不同时产生类型错误 |

## `NotCondition` 类

### 概述

表示 NOT 条件，公有继承 `Condition`。该类拥有一个子条件且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `operand_` | `Condition*` | 拥有的待取反条件 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `NotCondition` | 接收子条件并取得其所有权 |
| `evaluate` | 对子条件求值并返回相反结果 |

## `LogicalCondition` 类

### 概述

表示 AND 或 OR 条件，公有继承 `Condition`。该类拥有左右子条件且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `operation_` | `LogicalOperator` | 逻辑运算类型 |
| `left_` | `Condition*` | 拥有的左侧条件 |
| `right_` | `Condition*` | 拥有的右侧条件 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `LogicalCondition` | 接收逻辑运算类型和左右条件，并取得两个条件的所有权 |
| `evaluate` | 短路计算逻辑与或逻辑或，并返回判定结果 |
