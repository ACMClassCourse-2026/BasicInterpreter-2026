# Statement

Statement 是所有语句的多态基类，`execute` 返回 Continue、EndProgram 或
ExitFor。父语句拥有表达式、条件和子块裸指针并负责析构。
