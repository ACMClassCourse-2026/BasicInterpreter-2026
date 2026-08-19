# Statement

## 概述

语句节点组成程序语法树。父语句通过裸指针拥有表达式、条件和子语句块，并在析构时递归释放。所有语句通过 `ExecutionContext` 访问环境和输入输出，并使用 `ExecSignal` 向外传播控制流。

## `ExecSignal` 枚举类

| 值名称 | 含义 |
| :-: | :-: |
| `Continue` | 当前语句正常结束，继续执行下一条语句 |
| `EndProgram` | END 请求终止整个程序 |
| `ExitFor` | EXIT FOR 请求退出最近一层 FOR 循环 |

## `AssignmentTarget` 类

### 概述

表示 LET 或 INPUT 的赋值目标，可以是标量变量或数组元素。该类拥有全部下标表达式，不可复制但可以移动。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `name_` | `std::string` | 目标变量的名称 |
| `suffix_type_` | `std::optional<BasicType>` | 变量名后缀指定的类型；无后缀时为空 |
| `indices_` | `std::vector<Expression*>` | 拥有的下标表达式；为空时表示标量目标 |

### 私有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `clear` | 释放全部下标表达式并清空容器 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `AssignmentTarget` | 保存目标信息并取得所有下标表达式的所有权 |
| `~AssignmentTarget` | 释放拥有的下标表达式 |
| 移动构造与移动赋值 | 转移目标数据和下标表达式所有权 |
| `name` | 返回变量名 |
| `suffix_type` | 返回后缀指定的类型 |
| `indices` | 返回下标表达式列表的只读引用 |

## `Statement` 抽象类

所有语句节点的抽象基类，不可复制。虚析构函数保证通过基类指针正确释放子类对象。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `source_` | `std::string` | 该语句对应的原始源码，用于保留源码信息 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `Statement` | 保存语句的原始源码 |
| `execute` | 执行语句并返回控制流信号 |
| `source` | 返回原始源码的只读引用 |

## `RemStatement` 类

表示 REM 注释语句，没有额外数据成员。`execute` 不执行操作，直接返回 `Continue`。

## `DimStatement` 类

### 概述

表示 DIM 声明，可创建标量、定长数组或动态数组。该类拥有维度表达式和可选初值表达式。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `name_` | `std::string` | 待声明的变量名 |
| `declared_type_` | `std::optional<BasicType>` | 显式声明的类型；未指定时为空 |
| `dimensions_` | `std::vector<Expression*>` | 拥有的定长数组维度表达式 |
| `dynamic_` | `bool` | 是否声明动态数组 |
| `initializer_` | `Expression*` | 拥有的可选标量初值表达式，可为空 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `DimStatement` | 保存声明信息并取得维度和初值表达式的所有权 |
| `execute` | 计算声明类型、维度或初值，并在当前作用域创建对应变量 |

## `LetStatement` 类

### 概述

表示 LET 赋值语句，将表达式结果写入标量变量或数组元素。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `target_` | `AssignmentTarget` | 按值保存的赋值目标 |
| `expression_` | `Expression*` | 拥有的右侧表达式 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `LetStatement` | 取得赋值目标和右侧表达式的所有权 |
| `execute` | 计算下标和右侧表达式，检查后缀类型并完成赋值 |

## `PrintStatement` 类

### 概述

表示 PRINT 输出语句，拥有待输出表达式。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `expression_` | `Expression*` | 拥有的待输出表达式 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `PrintStatement` | 取得待输出表达式的所有权 |
| `execute` | 计算并格式化表达式，将结果和换行符写入输出流 |
| `expression` | 返回待输出表达式的只读引用 |

## `InputStatement` 类

### 概述

表示 INPUT 输入语句，从输入流读取一行并写入目标变量或数组元素。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `target_` | `AssignmentTarget` | 按值保存的输入目标 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `InputStatement` | 取得输入目标的所有权 |
| `execute` | 输出提示符并读取输入，按目标类型解析；非法数值会提示后重新读取 |

## `EndStatement` 类

表示 END 语句，没有额外数据成员。`execute` 返回 `EndProgram`，由外层语句块终止程序执行。

## `RedimStatement` 类

### 概述

表示 REDIM 或 REDIM PRESERVE 语句，用于重新分配一维动态数组。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `name_` | `std::string` | 目标数组名 |
| `length_` | `Expression*` | 拥有的新长度表达式 |
| `preserve_` | `bool` | 是否保留已有元素 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `RedimStatement` | 保存数组名和保留选项，并取得长度表达式所有权 |
| `execute` | 计算并检查新长度，然后重新分配动态数组 |

## `EraseStatement` 类

### 概述

表示 ERASE 语句，释放动态数组内容但保留其声明。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `name_` | `std::string` | 目标数组名 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `EraseStatement` | 保存目标数组名 |
| `execute` | 将动态数组恢复为未分配状态 |

## `BlockStatement` 类

### 概述

表示按顺序执行的一组语句，拥有其中的全部语句节点且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `statements_` | `std::vector<Statement*>` | 按源码顺序保存并拥有的语句节点 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `BlockStatement` | 创建空语句块，或取得给定语句列表的所有权 |
| `append` | 在末尾添加语句并取得其所有权 |
| `statements` | 返回语句列表的只读引用 |
| `execute` | 依次执行语句；遇到非 `Continue` 信号时立即向外返回 |

## `IfBranch` 结构体

表示 IF、ELSEIF 或 ELSE 的一个分支。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `condition` | `Condition*` | 拥有的分支条件；ELSE 分支为空指针 |
| `body` | `BlockStatement*` | 拥有的分支语句块 |

## `IfStatement` 类

### 概述

表示完整的 IF 结构，按顺序保存所有分支并拥有每个分支的条件和语句块。该类不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `branches_` | `std::vector<IfBranch>` | 按 IF、ELSEIF、ELSE 顺序保存的分支 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `IfStatement` | 取得全部分支及其子节点的所有权 |
| `execute` | 执行第一个条件成立的分支；被执行分支拥有独立局部作用域 |

## `ForStatement` 类

### 概述

表示 FOR 循环。起点、终点和步长在进入循环时各计算一次；控制变量存在于循环作用域中，每次循环体另有独立局部作用域。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `variable_` | `std::string` | 循环控制变量名 |
| `start_` | `Expression*` | 拥有的起点表达式 |
| `end_` | `Expression*` | 拥有的终点表达式 |
| `step_` | `Expression*` | 拥有的可选步长表达式；为空时使用同类型的 1 |
| `body_` | `BlockStatement*` | 拥有的循环体 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `ForStatement` | 保存控制变量并取得起点、终点、步长和循环体的所有权 |
| `execute` | 按步长方向执行循环；处理 `ExitFor`，并向外传播程序结束信号 |

## `WhileStatement` 类

### 概述

表示 WHILE 循环，在每轮开始前判断条件，每次循环体执行都拥有独立局部作用域。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `condition_` | `Condition*` | 拥有的循环条件 |
| `body_` | `BlockStatement*` | 拥有的循环体 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `WhileStatement` | 取得循环条件和循环体的所有权 |
| `execute` | 条件成立时反复执行循环体，并将非 `Continue` 信号向外传播 |

## `ExitForStatement` 类

表示 EXIT FOR 语句，没有额外数据成员。`execute` 返回 `ExitFor`，由最近一层 `ForStatement` 消费该信号并结束循环。
