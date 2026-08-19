# ArrayValue

## 概述

ArrayValue 类用于存储数组类型，涵盖了一维或高维定长数组，以及一维动态数组的处理。具体的存储方法是把数组拍平成一维，存入 `std::vector` 中。

## 数据成员

| 名称 | 类型 | 作用 |
| :-: | :-: | :-: |
| `element_type_` | `BasicType` | 数组元素的类型（INTEGER, DOUBLE 等）|
| `dynamic_` | `bool` | 是否是动态数组 |
| `dimensioned_` | `bool` | 动态数组是否已被分配大小（对静态数组无效） |
| `dimensions_` | `std::vector<std::size_t>` | 数组维数（例如，arr(3, 4) 对应 dimensions_ 存储 {3, 4}） |
| `elements_` | `std::vector<Value>` | 数组内容（拍平成一维） |

## 私有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `flat_index` | 把高维数组下标转换为一维下标 |

## 公有成员函数

| 名称 | 作用 |
| :-: | :-: |
| `element_type` | 返回成员类型 |
| `is_dynamic` | 返回是否为动态数组 |
| `is_dimentioned` | 返回动态数组是否已经被分配大小 |
| `dimensions` | 返回数组维数 |
| `at` | 根据传入下标返回数组元素 |
| `redim` | 重新分配动态数组大小，根据参数 `preserve` 决定是否保留已有元素 |
| `erase` | 释放动态数组空间 |
