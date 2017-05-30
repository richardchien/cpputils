# C++ Utilities

[![License](https://img.shields.io/badge/license-MIT%20License-blue.svg)](LICENSE)

一些简易的 C++ 工具函数和类。

注意：目前只在 Windows 10 上使用 Visual Studio 2017 (v141) 工具集测试过。

## `str` 类

模仿 Python 的 `str` 类，实现了部分方法，使用方法和 Python 的基本一致，抛出异常也基本一致，并且实现无缝的和 C++ `string` 和 `wstring` 的转换。主要代码在 [`str.h`](cpputils/include/str.h)、[`str.cpp`](cpputils/src/str.cpp)。

理论上这个类可以跨平台使用，不过并没有测试过。

## 示例

```cpp
using namespace rc;

str s = u8"这是 一段 文字 ";
s.length() == 9;
s[0] == u8"这";
s[slice(1, 5)] == u8"是 一段";
s.strip() == u8"这是 一段 文字";
s.split() == std::vector<str>{u8"这是", u8"一段", u8"文字"};
s.startswith(u8"这") == true;
s.replace(" ", "/") == u8"这是 一段 文字";

auto i = int(str("123"));
i == 123;
auto f = float(str("1.23"));
f == 1.23f;
```

其它的具体方法签名见 [`str.h`](cpputils/include/str.h)。

## 字符编码相关函数

提供了 `encode` 和 `decode` 函数来对字符串进行编码和解码，主要代码在 [`win32_bytes.h`](cpputils/include/win32_bytes.h)、[`win32_encoding.h`](cpputils/include/win32_encoding.h)。这两个函数只能在 Windows 上使用。

## 示例

```cpp
using namespace rc;
using namespace rc::win32;

bytes b = "你好" // 其它编码的字符串，假设此处为 GBK
str s = decode(b, Encoding::GBK);
s == u8"你好";

auto big5_bytes = encode(str(u8"測試"), Encoding::BIG5);
```
