# BASIC Interpreter 2026 语法说明

## 程序与行号

BASIC 程序由若干条带行号的语句组成。行号是 `int32_t` 范围内的非负整数，程序按行号从小到大执行，行号不必连续。

```BASIC
10 REM A + B
20 DIM A AS INTEGER
30 DIM B AS INTEGER
40 INPUT A
50 INPUT B
60 PRINT A + B
70 END
```

向解释器输入带行号的语句，会把它加入当前程序：

- 新行号：插入语句；
- 已有行号：覆盖原语句；
- 只有行号：删除该行；
- 无行号的简单语句：立即执行。

```BASIC
20 DIM X AS INTEGER
20 DIM X AS LONG
20
PRINT 1 + 2
```

结构化语句不能立即执行。立即模式只接受 `REM`、`DIM`、`LET`、`PRINT`、`INPUT`、`REDIM` 和 `ERASE`。

## 关键字与标识符

关键字必须大写。标识符以字母开头，之后可以包含字母或数字，并且区分大小写。

```BASIC
DIM score AS INTEGER
DIM Score AS INTEGER
```

上例中的 `score` 和 `Score` 是两个不同的变量。

标识符末尾可以使用类型后缀。后缀只表示类型，不属于变量的基础名称，因此 `X` 和 `X%` 指向同一个变量。引用变量时可以省略后缀；如果写出后缀，它必须与变量的声明类型一致。

## 数据类型

解释器支持五种数据类型：

| BASIC 类型 | C++ 类型 | 后缀 | 示例 |
| :-: | :-: | :-: | :-: |
| `INTEGER` | `int32_t` | `%` | `COUNT%` |
| `LONG` | `int64_t` | `&` | `SIZE&` |
| `SINGLE` | `float` | `!` | `PRICE!` |
| `DOUBLE` | `double` | `#` | `PI#` |
| `STRING` | `std::string` | `$` | `NAME$` |

字面量按以下规则推断类型：

- 无小数点的整数优先推断为 `INTEGER`，超出其范围但仍在 `int64_t` 范围内时推断为 `LONG`；
- 含小数点的数字推断为 `SINGLE`；
- 双引号包围的文本推断为 `STRING`。

```BASIC
DIM I = 42
DIM BIG = 1000000000000
DIM RATE = 3.5
DIM NAME = "Alice"
```

## 变量声明与赋值

### DIM：声明变量

变量必须先用 `DIM` 声明，才能读取或赋值。

```BASIC
DIM AGE AS INTEGER
DIM PRICE!
DIM PI# = 3.141592653589793
DIM NAME$ = "Alice"
```

标量声明的完整形式为：

```BASIC
DIM name [AS type] [= expression]
```

类型可以由后缀或 `AS` 指定。两者同时出现时必须一致：

```BASIC
DIM TOTAL& AS LONG
```

如果没有显式类型：

- 有初值时，从初值推断类型；
- 没有初值时，默认为 `SINGLE`。

未提供初值时，数值变量初始化为 `0`，字符串初始化为空字符串。

同一作用域内不能重复声明同名变量，但内层作用域可以遮蔽外层变量。

### LET：赋值

`LET` 只能给已经声明的变量或数组元素赋值。

```BASIC
DIM X AS INTEGER
LET X = 10
```

数值赋值允许在四种数值类型之间转换，转换前必须检查范围；浮点数转换为整数时向零截断。字符串只能赋给字符串。

## 类型转换函数

以下函数用于显式数值转换：

| 函数 | 返回类型 |
| :-: | :-: |
| `CINT(expr)` | `INTEGER` |
| `CLNG(expr)` | `LONG` |
| `CSNG(expr)` | `SINGLE` |
| `CDBL(expr)` | `DOUBLE` |

```BASIC
DIM A AS INTEGER = 10
DIM B AS LONG = 20
PRINT CLNG(A) + B
```

转换函数只接受数值参数，转换结果超出目标类型范围时报错。

## 数组

数组长度和下标表达式必须是整数。数组采用从 `0` 开始的下标，`DIM A(10)` 表示 10 个元素，合法下标为 `0` 到 `9`。

### 定长数组

基础任务支持一维和高维定长数组：

```BASIC
DIM VALUES(20) AS DOUBLE
DIM MATRIX%(3, 4)

LET VALUES(0) = 1.5
LET MATRIX(2, 3) = 7
PRINT MATRIX(2, 3)
```

`MATRIX(3, 4)` 是一个 `3 × 4` 的数组。数组创建后，所有元素使用对应类型的默认值初始化。定长数组不能使用 `REDIM` 或 `ERASE`。

### 可变长数组

基础任务只要求一维可变长数组：

```BASIC
DIM VEC() AS INTEGER
REDIM VEC(10)
LET VEC(1) = 5

REDIM VEC(20)
REM 原有内容被丢弃

REDIM PRESERVE VEC(40)
REM 保留原数组与新数组共同范围内的内容

ERASE VEC
```

动态数组在首次 `REDIM` 前不能访问。`ERASE` 释放其内容，但保留数组声明，之后可以再次 `REDIM`。

## 表达式

### 算术运算

| 运算符 | 含义 |
| :-: | :-: |
| `+` | 加法或字符串连接 |
| `-` | 减法或取负 |
| `*` | 乘法 |
| `/` | 浮点除法 |
| `\` | 整数除法 |
| `MOD` | 取模 |
| `^` | 幂运算 |

二元运算的两个操作数必须具有相同类型，不进行自动类型提升。类型不同的操作数需要先使用转换函数。

```BASIC
DIM A AS INTEGER = 7
DIM B AS INTEGER = 2
PRINT A / B
PRINT A \ B
PRINT A MOD B
```

两个整数进行 `/` 运算时，结果为 `SINGLE`。整数除法和取模只接受 `INTEGER` 或 `LONG`。字符串只支持 `+` 和比较运算。

整数幂的指数必须是非负整数。所有整数运算都需要检查溢出。

### 比较与逻辑运算

比较运算符包括：

```text
=  <>  >  <  >=  <=
```

比较的两个操作数必须同类型；字符串按字典序比较。

逻辑运算符包括 `NOT`、`AND` 和 `OR`。`AND`、`OR` 采用短路求值。逻辑表达式用于 IF 和 WHILE 的条件。

```BASIC
IF SCORE >= 60 AND SCORE <= 100 THEN
    PRINT "PASS"
END IF
```

运算符优先级从高到低为：

1. 函数调用、数组下标和括号；
2. `^`，从右向左结合；
3. 一元 `+`、一元 `-`；
4. `*`、`/`、`\`、`MOD`；
5. `+`、`-`；
6. 比较运算；
7. `NOT`；
8. `AND`；
9. `OR`。

## 顺序语句

### REM：注释

`REM` 后面的内容不会被解析或执行，空注释也合法。

```BASIC
10 REM This is a comment.
20 REM
```

### PRINT：输出

`PRINT expression` 计算并输出表达式的值，然后换行。

```BASIC
PRINT "Hello, " + "BASIC"
```

### INPUT：输入

`INPUT target` 输出提示符 `? `，读取一行并写入变量或数组元素。输入必须能完整转换为目标类型；非法数值输入会输出 `INVALID NUMBER` 并重新读取。

```BASIC
DIM AGE AS INTEGER
INPUT AGE
```

### END：结束程序

执行 `END` 后立即结束本次 `RUN`。

## 分支语句

IF 可以包含任意多个 `ELSEIF` 和至多一个 `ELSE`，最后必须使用 `END IF` 结束。

```BASIC
10 DIM SCORE AS INTEGER
20 INPUT SCORE
30 IF SCORE >= 95 THEN
40   PRINT "4.3"
50 ELSEIF SCORE >= 90 THEN
60   PRINT "4.0"
70 ELSE
80   PRINT "<4.0"
90 END IF
100 END
```

解释器只执行第一个条件成立的分支。被执行的分支拥有独立的内层作用域。

## 循环语句

### FOR 循环

```BASIC
10 FOR I = 1 TO 10
20   PRINT I
30 NEXT I
```

循环包含终点。可以使用 `STEP` 指定步长：

```BASIC
10 FOR I = 10 TO 1 STEP -1
20   PRINT I
30 NEXT I
```

起点、终点和步长在进入循环时各计算一次。步长不能为零。FOR 自动在循环作用域内声明控制变量，该变量在循环结束后不可见。

`NEXT` 后的变量名必须与对应的 FOR 控制变量一致。

### EXIT FOR

`EXIT FOR` 退出最近一层 FOR 循环，也可以出现在循环内嵌套的 IF 或 WHILE 中。

```BASIC
10 FOR I = 1 TO 100
20   IF I = 20 THEN
30     EXIT FOR
40   END IF
50 NEXT I
```

### WHILE 循环

```BASIC
10 DIM X AS INTEGER = 0
20 WHILE X < 10
30   LET X = X + 1
40   PRINT X
50 WEND
```

### DO 循环*

#### 前置条件

```BASIC
10 DIM X AS INTEGER = 0
20 DO WHILE X < 10
30   LET X = X + 1
40   PRINT X
50 LOOP
```

```BASIC
10 DIM X AS INTEGER = 0
20 DO UNTIL X >= 10
30   LET X = X + 1
40   PRINT X
50 LOOP
```

#### 后置条件

```BASIC
10 DIM X AS INTEGER = 0
20 DO
30   LET X = X + 1
40   PRINT X
50 LOOP WHILE X < 10
```

```BASIC
10 DIM X AS INTEGER = 0
20 DO
30   LET X = X + 1
40   PRINT X
50 LOOP UNTIL X >= 10
```

#### 提前退出

```BASIC
10 DIM X AS INTEGER
20 DO
30   INPUT X
40   IF X = 0 THEN
50     EXIT DO
60   END IF
70   PRINT X
80 LOOP
```

WHILE 在每轮开始前计算条件。每次执行循环体都会进入新的内层作用域。

## 作用域

IF 分支和循环体都会创建内层作用域，其查找规则与 C++ 块作用域相似：

- 读取变量时从当前层向外查找；
- `LET` 修改最近的同名声明；
- `DIM` 在当前层创建变量，可以遮蔽外层同名变量；
- 离开代码块后，该块内声明的变量和数组被销毁。

```BASIC
10 DIM X AS INTEGER = 1
20 IF X = 1 THEN
30   DIM X AS INTEGER = 2
40   PRINT X
50 END IF
60 PRINT X
```

上例依次输出 `2` 和 `1`。

## 解释器指令

| 指令 | 作用 |
| :-: | :-: |
| `RUN` | 从最小行号开始执行程序 |
| `LIST` | 按行号升序列出当前程序 |
| `CLEAR` | 清空程序和变量状态 |
| `QUIT` | 退出解释器 |
| `HELP` | 输出帮助信息，不参与评测 |

每次 `RUN` 在程序结构检查成功后清空变量状态，再从头执行。运行结束后，全局变量可以被立即语句访问；下一次 `RUN` 会再次清空它们。

程序中的词法或单行语法错误在输入该行时报告；缺少 `END IF`、`NEXT`、`WEND` 等跨行结构错误在 `RUN` 时报告。

运行时错误会终止本次 `RUN`，但不会退出解释器。

## 错误信息

解释器至少需要支持以下稳定错误信息：

| 错误信息 | 含义 |
| :-: | :- |
| `SYNTAX ERROR` | 词法、语法或程序块结构错误 |
| `DIVIDE BY ZERO` | 除数为零 |
| `INVALID NUMBER` | INPUT 得到非法数值 |
| `VARIABLE NOT DEFINED` | 使用了未声明或不可见的变量 |
| `VARIABLE ALREADY DEFINED` | 在同一作用域重复声明变量 |
| `TYPE MISMATCH` | 操作数或赋值类型不符合要求 |
| `OVERFLOW` | 字面量、运算或类型转换溢出 |
| `INVALID ARRAY SIZE` | 数组长度非法或容量溢出 |
| `ARRAY NOT DIMENSIONED` | 动态数组尚未分配 |
| `SUBSCRIPT OUT OF RANGE` | 数组下标越界 |
| `INVALID REDIM` | 对不允许调整的数组使用 REDIM/ERASE |
| `STEP CANNOT BE ZERO` | FOR 的步长为零 |
