# Value

## 概述

`Value` 表示一个 BASIC 标量值，使用 `std::variant` 按值保存五种受支持的类型。二元运算要求两侧类型相同，不执行隐式类型提升；数值运算和转换负责检查类型、除零和溢出。字符串不参与数值转换，只支持连接和比较。

## `BasicType` 枚举类

| 值名称 | C++ 存储类型 | 类型后缀 |
| :-: | :-: | :-: |
| `Integer` | `std::int32_t` | `%` |
| `Long` | `std::int64_t` | `&` |
| `Single` | `float` | `!` |
| `Double` | `double` | `#` |
| `String` | `std::string` | `$` |

## `Value` 类

### 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `data_` | `std::variant<std::int32_t, std::int64_t, float, double, std::string>` | 保存当前值；当前候选项同时决定 BASIC 类型 |

### 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `Value` | 默认构造 `Single` 类型的 0，或根据给定 C++ 值构造对应类型的 BASIC 值 |
| `default_value` | 返回指定类型的默认值：数值类型为 0，字符串为空串 |
| `type` | 返回当前值的 `BasicType` |
| `is_numeric` | 判断当前值是否为四种数值类型之一 |
| `as_integer` | 读取 `Integer` 值；当前类型不符时产生类型错误 |
| `as_long` | 读取 `Long` 值；当前类型不符时产生类型错误 |
| `as_single` | 读取 `Single` 值；当前类型不符时产生类型错误 |
| `as_double` | 读取 `Double` 值；当前类型不符时产生类型错误 |
| `as_string` | 返回 `String` 值的只读引用；当前类型不符时产生类型错误 |
| `convert_to` | 执行显式数值类型转换并检查范围；不支持数值与字符串互转 |
| `format` | 按 BASIC 输出规则将值格式化为 PRINT 使用的字符串 |

## 自由函数

以下二元运算函数都要求左右值类型相同。

| 名称 | 作用 |
| :-: | :-: |
| `add_values` | 执行加法；字符串参数执行连接 |
| `subtract_values` | 执行数值减法 |
| `multiply_values` | 执行数值乘法 |
| `divide_values` | 执行浮点除法；两个整数相除时结果为 `Single` |
| `integer_divide_values` | 对 `Integer` 或 `Long` 执行整数除法 |
| `modulo_values` | 对 `Integer` 或 `Long` 执行取余 |
| `power_values` | 执行乘方；整数指数必须为非负数 |
| `negate_value` | 对数值取负并检查溢出 |
| `compare_values` | 比较同类型值；返回负数、0 或正数表示左值小于、等于或大于右值 |
