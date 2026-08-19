# Expression

## 概述

表达式使用树形结构保存。父节点通过裸指针拥有子表达式并在析构时递归释放；所有 `evaluate` 函数只借用 `Environment`，不会修改运行环境。

## 枚举类

### `ArithmeticOperator`

| 值名称 | 含义 |
| :-: | :-: |
| `Add` | 加法或字符串连接 |
| `Subtract` | 减法 |
| `Multiply` | 乘法 |
| `Divide` | 普通除法 |
| `IntegerDivide` | 整数除法 |
| `Modulo` | 取余 |
| `Power` | 乘方 |

### `ConversionKind`

| 值名称 | 含义 |
| :-: | :-: |
| `CInt` | 转换为 `Integer` |
| `CLng` | 转换为 `Long` |
| `CSng` | 转换为 `Single` |
| `CDbl` | 转换为 `Double` |

## `Expression` 抽象类

所有表达式节点的抽象基类。虚析构函数保证通过基类指针正确释放子类对象。

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `evaluate` | 在给定运行环境中计算并返回表达式值 |

## `LiteralExpression` 类

表示字面量，是表达式树的叶子节点。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `value_` | `Value` | 字面量的值 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `LiteralExpression` | 按值保存给定字面量 |
| `evaluate` | 返回保存的字面量值 |

## `VariableExpression` 类

表示标量变量读取或数组元素读取。该类拥有全部下标表达式且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `name_` | `std::string` | 变量名 |
| `suffix_type_` | `std::optional<BasicType>` | 变量名后缀指定的类型；无后缀时为空 |
| `indices_` | `std::vector<Expression*>` | 拥有的数组下标表达式；为空时表示标量访问 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `VariableExpression` | 保存变量信息并取得所有下标表达式的所有权 |
| `evaluate` | 计算下标并读取变量；若类型后缀与实际类型不符则产生类型错误 |

## `UnaryExpression` 类

表示一元正号或负号。该类拥有操作数且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `negative_` | `bool` | 是否执行取负；为 `false` 时表示一元正号 |
| `operand_` | `Expression*` | 拥有的操作数表达式 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `UnaryExpression` | 保存正负号信息并取得操作数所有权 |
| `evaluate` | 计算操作数并按需取负，仅接受数值类型 |

## `BinaryExpression` 类

表示二元算术表达式。该类拥有左右操作数且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `operation_` | `ArithmeticOperator` | 算术运算类型 |
| `left_` | `Expression*` | 拥有的左操作数 |
| `right_` | `Expression*` | 拥有的右操作数 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `BinaryExpression` | 保存运算类型并取得左右操作数的所有权 |
| `evaluate` | 依次计算左右操作数并执行指定算术运算 |

## `ConversionExpression` 类

表示显式数值类型转换。该类拥有操作数且不可复制。

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `kind_` | `ConversionKind` | 目标转换类型 |
| `operand_` | `Expression*` | 拥有的待转换表达式 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `ConversionExpression` | 保存转换类型并取得操作数所有权 |
| `evaluate` | 计算操作数并转换为枚举值指定的数值类型 |
