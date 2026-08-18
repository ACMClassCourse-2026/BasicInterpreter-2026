# Parser

`parse_input` 区分空行、解释器指令、程序行编辑、删除和立即语句，并在插入前
完成单行语法检查。`parse_program` 对有序源码进行跨行块匹配，返回调用者拥有
的 ProgramAst；解析异常时不得泄漏已构造节点。
