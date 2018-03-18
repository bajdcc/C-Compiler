# C语言解析器（MFC）

这是一个MFC工程，分为IDE、LOGIC、UI部分，各部分通过DLL来调用。

**代码来源：[陆晓春 - C语言编译器](https://wenku.baidu.com/view/98e4b90ff12d2af90242e619.html)。**我只是把代码整理了一下，修改了些BUG，只完成语法树解析功能，没做生成目标代码功能。

## 模块

- IDE主要负责MFC窗口的展现。
- LOGIC负责C语言解析，功能分为词法解析、语法解析、类型检查、变量符号表。
- UI负责Edit文本编辑器的语法高亮及编辑功能。

## 截图

![Screenshot](https://github.com/bajdcc/C-Compiler/raw/master/screenshot/1.png)
![Screenshot](https://github.com/bajdcc/C-Compiler/raw/master/screenshot/2.png)