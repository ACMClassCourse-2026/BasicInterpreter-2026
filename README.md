# Basic-Interpreter-2026

## 简介

### 背景

BASIC 是一门**解释性**编程语言，本次大作业要求你用 C++ 实现这个语言中的一些功能，来完成一个 BASIC 解释器。

该解释器具有的功能有：

- 立即解释语句并运行
- 执行特定控制语句
- 按照行数升序运行程序

### 作业目标

- 锻炼文档的阅读能力
- 了解解释器的设计
- 锻炼 git 的使用

## 作业要求

### 基础任务

#### 下发文件

```
.
├── CMakeLists.txt
├── README.md
├── SYNTAX.md
├── docs
│   ├── array_value.md
│   ├── basic_error.md
│   ├── condition.md
│   ├── environment.md
│   ├── execution_context.md
│   ├── expression.md
│   ├── interpreter.md
│   ├── lexer.md
│   ├── parser.md
│   ├── program.md
│   ├── program_ast.md
│   ├── program_store.md
│   ├── scope_guard.md
│   ├── source_line.md
│   ├── statement.md
│   ├── token.md
│   └── value.md
├── include
│   └── basic
│       ├── array_value.hpp
│       ├── basic_error.hpp
│       ├── condition.hpp
│       ├── environment.hpp
│       ├── execution_context.hpp
│       ├── expression.hpp
│       ├── interpreter.hpp
│       ├── lexer.hpp
│       ├── parser.hpp
│       ├── program.hpp
│       ├── program_ast.hpp
│       ├── program_store.hpp
│       ├── scope_guard.hpp
│       ├── source_line.hpp
│       ├── statement.hpp
│       ├── token.hpp
│       └── value.hpp
├── run_tests.sh
├── src
│   ├── basic_error.cpp
│   ├── expression.cpp
│   ├── lexer.cpp
│   ├── main.cpp
│   ├── parser.cpp
│   └── token.cpp
└── tests
    └── cases
```


#### 解释器实现说明

需要实现的语法以及解释器要求详见[文档](SYNTAX.md)。

你需要添加或补全 `src/` 下的源文件。

#### 实现要求

我们会进行鲁棒性检测和内存泄漏检测，但是不进行性能检测，你只需要通过下发的数据点即可。

## 须知

### Deadline
TBD

### 本地评测

#### 评测数据

`test/cases/` 文件夹中的 400 个数据点。

#### 评测脚本使用方法

运行 `run_tests.sh` 即可。

### OJ 评测

你需要使用 git 进行版本管理和代码提交。你只需要在 OJ 上提交你的仓库地址（如 `https://github.com/<name>/<repo>`）即可。

注意，你的可执行文件的名字**必须为 `code`**。

### 评分细则

| 任务 | 占比 |
| :-: | :-: |
| 解释器 | 80% |
| Bonus | 至多 20% |
| Code Review | 20% |

## Bonus

（暂定，具体文档待补全）

- 实现更加完整的字符串工具函数 2%
- 实现 DO 循环 3%
- 实现 SWITCH 跳转 3%
- 实现 SUB 过程 5%
- 实现 FUNCTION 函数调用 5%
- 实现 TYPE 自定义数据类型 5%
- 实现 ON ERROR 错误处理 5%

## 负责助教

TBD

## Special Thanks

TBD