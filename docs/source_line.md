# SourceLine

## 概述

`SourceLine` 表示一行已保存的 BASIC 程序。`ProgramStore` 返回按行号排序的 `SourceLine` 列表，`Parser` 在 RUN 时使用这些对象重新构建语法树。

## 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `line_number` | `std::int32_t` | BASIC 程序行号，默认为 0 |
| `source` | `std::string` | 不含行号且未经改写的原始源码 |

`SourceLine` 没有成员函数。
