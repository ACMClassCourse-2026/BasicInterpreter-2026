# SourceLine

`SourceLine` 按值保存一个非负 BASIC 行号及该行未经重写的源码。ProgramStore
返回按行号排序的 SourceLine，Parser 在 RUN 时消费这些对象并重新构建 AST。
