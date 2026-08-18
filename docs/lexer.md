# Lexer

Lexer 将一行源码转换为 TokenStream。关键字只识别大写形式，普通标识符保持
大小写；REM 后内容作为单个 RemText；非法字符和未闭合字符串产生语法错误。
