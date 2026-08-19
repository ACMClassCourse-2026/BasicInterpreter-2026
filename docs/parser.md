# Parser

## 概述

`Parser` 将用户输入解析为解释器命令、程序行操作或语句，并在 RUN 时将有序源码构建为完整语法树。解析器自身不保存状态。

## 枚举类

### `InterpreterCommand`

| 值名称 | 含义 |
| :-: | :-: |
| `Run` | 运行当前程序 |
| `List` | 按行号列出当前程序 |
| `Clear` | 清空程序和运行环境 |
| `Quit` | 退出解释器 |
| `Help` | 显示帮助信息 |

### `ParsedInputKind`

| 值名称 | 含义 |
| :-: | :-: |
| `Empty` | 空输入 |
| `Command` | 解释器命令 |
| `ProgramLine` | 新增或覆盖一行程序 |
| `DeleteLine` | 删除指定程序行 |
| `ImmediateStatement` | 立即执行的语句 |

## `ParsedInput` 类

### 概述

表示一行用户输入的解析结果。对象只使用与 `kind_` 对应的字段；立即语句由该对象拥有。该类不可复制，但可以移动。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `kind_` | `ParsedInputKind` | 当前解析结果的类别 |
| `command_` | `InterpreterCommand` | 命令类别，仅供命令输入使用 |
| `line_number_` | `std::int32_t` | 程序行号，仅供程序行新增、覆盖或删除使用 |
| `source_` | `std::string` | 不含行号的原始程序源码 |
| `statement_` | `Statement*` | 拥有的立即语句；其他类别为空指针 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `ParsedInput` | 创建类别为 `Empty` 的解析结果 |
| `~ParsedInput` | 释放尚未移交的立即语句 |
| 移动构造与移动赋值 | 转移字符串和立即语句所有权，并使源对象不再拥有语句 |
| `command` | 创建命令解析结果 |
| `program_line` | 创建程序行新增或覆盖结果 |
| `delete_line` | 创建程序行删除结果 |
| `immediate` | 创建立即语句结果并取得语句所有权 |
| `kind` | 返回解析结果类别 |
| `command_value` | 返回命令类别；当前结果不是命令时产生逻辑错误 |
| `line_number` | 返回程序行号；当前结果不含行号时产生逻辑错误 |
| `source` | 返回保存的程序源码 |
| `release_statement` | 将立即语句所有权移交给调用者，并将内部指针置空 |

## `Parser` 类

### 私有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `parse_simple_statement` | 从 token 流解析一条不包含跨行结构的语句，返回调用者拥有的语句节点 |
| `parse_expression` | 按运算符优先级解析表达式；`minimum_precedence` 表示当前允许的最低优先级 |
| `parse_condition` | 解析比较、NOT、AND 和 OR 组成的条件表达式 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `parse_input` | 区分空行、命令、程序行新增或删除和立即语句，并返回对应解析结果 |
| `parse_program` | 对按行号排序的源码进行跨行块匹配，返回调用者拥有的 `ProgramAst` |
