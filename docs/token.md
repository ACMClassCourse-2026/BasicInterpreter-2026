# Token 与 TokenStream

`Token` 保存类型、原始文本和从零开始的列号，包括五种变量类型后缀。
`TokenStream` 拥有 token 序列并提供查看、消费、匹配、断言和复位操作；读取
结尾时返回显式的 EndOfInput。
