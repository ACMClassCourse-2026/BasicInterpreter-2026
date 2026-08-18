# ProgramAst

ProgramAst 独占完整程序的根 BlockStatement，不可复制并可安全移动。它的析构
会递归释放全部语句、条件和表达式。Program 在每次 RUN 后都必须释放该对象。
