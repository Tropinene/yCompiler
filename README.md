# A yCompiler
本项⽬是基于flex和bison，使⽤C语言实现的类C语法编译器，使⽤flex与yacc对源代码进行词法、语法分析；在语法分析阶段生成与源代码对应的抽象语法树后，根据自行定义的中间代码语法输出中间语⾔代码；最后，本项⽬将中间代码转换成可运行的x86汇编代码。同时也实现了语法树的可视化输出。

## 开发环境
* 操作系统：macOS、Windows、Linux
* 编译环境：Flex、Bison、gcc
* 汇编代码运行环境：在线模拟器或支持x86汇编运行的系统

## 支持的操作与数据类型
* 变量的声明和初始化
* 全局变量的使用
* 函数的声明和调用（含参数、无参数）
* if-else 、if、while等控制流语句
* 赋值语句
* 二元运算
* 单行注释
* 基本数据类型（int）

## 文件说明
* main.c: 主函数所在文件，调⽤词法分析器、语法分析器、代码⽣成器
* makefile: 定义各文件编译和链接的规则
* lex.l: Flex源代码，实现词法分析，生成Tokens
* grammar.y: Yacc源代码，实现语法分析，生成抽象语法树
* ast.h: 定义抽象语法树的节点及节点类型
* ast.c: 生成抽象语法树的节点，绘制语法树
* genCode.h: 定义生成中间代码的函数
* genCode.c: 生成中间代码
* funcStack.h: 函数定义所需要的一些全局变量
* IRexplain.py: 将中间代码翻译成x86汇编
* test.c: 提供的测试样例

## 操作说明
进入yCompiler目录后，使用make命令生成可执行文件minic
生成语法树：./minic -a + 测试文件
生成中间代码：./minic -i + 测试文件
中间代码转换成汇编代码：python3 IRexplainnew.py

eg.
```shell
$ cd ./yCompiler
$ make
$ ./minic -a test.c
$ ./minic -i test.c
$ python3 IRexplainnew.py
```
其中可视化语法树会生成在 `tree.gv` 中，中间代码默认生成为 `a.ir`，汇编语言文件默认为 `a.asm`

由于汇编器把相对路径直接写在python代码里，建议自行修改成可接受输入的格式。