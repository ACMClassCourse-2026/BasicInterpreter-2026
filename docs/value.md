# Value

`Value` 使用 `std::variant` 按值保存 INTEGER、LONG、SINGLE、DOUBLE 或 STRING。
它负责默认值、显式数值转换、同类型运算、比较、溢出检测和 PRINT 格式化。
字符串不参与数值转换，只支持连接和比较。
