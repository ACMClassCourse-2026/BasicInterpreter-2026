# Token 与 TokenStream

## 概述

`Token` 表示词法分析得到的单个记号，`TokenStream` 拥有一行源码对应的 token 序列并维护当前读取位置。序列始终以显式的 `EndOfInput` 结束。

## `TokenType` 枚举类

### 基础记号

| 值名称 | 含义 |
| :-: | :-: |
| `EndOfInput` | 输入结束标记 |
| `Identifier` | 普通标识符 |
| `IntegerLiteral` | 不含小数点的整数字面量 |
| `RealLiteral` | 含小数点的实数字面量 |
| `StringLiteral` | 双引号包围的字符串内容 |
| `RemText` | REM 后不再继续分词的注释文本 |

### 关键字记号

| 值名称 | 含义 |
| :-: | :-: |
| `Dim`、`As`、`Let`、`Print`、`Input`、`End`、`Rem` | 对应同名的声明、赋值、输入输出、结束和注释关键字 |
| `Redim`、`Preserve`、`Erase` | 对应动态数组操作关键字 |
| `If`、`Then`、`ElseIf`、`Else` | 对应条件分支关键字 |
| `For`、`To`、`Step`、`Next`、`Exit` | 对应 FOR 循环及 EXIT FOR 关键字 |
| `While`、`Wend` | 对应 WHILE 循环关键字 |
| `Run`、`List`、`Clear`、`Quit`、`Help` | 对应解释器命令 |
| `Integer`、`Long`、`Single`、`Double`、`String` | 对应五种 BASIC 类型关键字 |
| `CInt`、`CLng`、`CSng`、`CDbl` | 对应四种显式数值转换函数 |
| `Mod` | 取余运算符 |
| `Not`、`And`、`Or` | 逻辑运算符 |

### 符号记号

| 值名称 | 含义 |
| :-: | :-: |
| `Plus`、`Minus`、`Multiply` | `+`、`-`、`*` |
| `Divide`、`IntegerDivide`、`Power` | `/`、`\\`、`^` |
| `Equal`、`NotEqual` | `=`、`<>` |
| `Greater`、`Less`、`GreaterEqual`、`LessEqual` | `>`、`<`、`>=`、`<=` |
| `LeftParen`、`RightParen`、`Comma` | `(`、`)`、`,` |
| `PercentSuffix` | INTEGER 类型后缀 `%` |
| `AmpersandSuffix` | LONG 类型后缀 `&` |
| `ExclamationSuffix` | SINGLE 类型后缀 `!` |
| `HashSuffix` | DOUBLE 类型后缀 `#` |
| `DollarSuffix` | STRING 类型后缀 `$` |

## `Token` 结构体

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `type` | `TokenType` | token 类别，默认为 `EndOfInput` |
| `text` | `std::string` | token 对应的原始文本；字符串字面量不包含外层双引号 |
| `column` | `std::size_t` | token 起始位置的从零开始列号 |

## `TokenStream` 类

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `tokens_` | `std::vector<Token>` | 拥有的完整 token 序列 |
| `position_` | `std::size_t` | 下一个待读取 token 的下标 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `TokenStream` | 创建空 token 流，或取得给定序列的所有权并保证末尾存在 `EndOfInput` |
| `peek` | 查看当前位置之后指定偏移量的 token，不改变读取位置；越过末尾时返回结束标记 |
| `consume` | 返回当前 token；若不是结束标记则将读取位置后移 |
| `match` | 当前 token 类型匹配时消费并返回 `true`，否则返回 `false` |
| `expect` | 要求并消费指定类型；不匹配时产生语法错误 |
| `at_end` | 判断当前位置是否已经到达输入末尾 |
| `position` | 返回当前读取位置 |
| `reset` | 将读取位置恢复到序列开头 |
| `tokens` | 返回完整 token 序列的只读引用 |

## 自由函数

| 名称 | 作用 |
| :-: | :-: |
| `token_type_name` | 返回 token 类型对应的可读名称或符号文本 |
