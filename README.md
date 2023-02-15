# SpaceEngine 0.990 详细信息生成器

协议：[GPLv2](https://www.gnu.org/licenses/licenses.html)，[CC BY-SA 4.0](https://creativecommons.org/licenses)

上一个详细信息生成器链接：[百度贴吧搬运链接，SE论坛的原链接不知去向](https://tieba.baidu.com/p/4458618242)

这个工程为上述版本的C++重写，基于[CSpaceEngine](https://github.com/StellarDX/CSpaceEngine-Project)，
输出格式为Github Markdown(.md)，因为作者不会网页前端，不过以后可能会支持。

(这还是一个究极不完整的版本)

## 编译
先编译CSpaceEngine，然后打开工程的sln文件，右键DetailedInfoGen项目，转到属性->VC++目录，
将外部包含目录和库目录修改为CSE的头文件目录(/headers)，以及CSE本体(CSE.lib)所在的目录，然后再生成就可以了。

## 使用方法
1. 打开SE，选中目标系统
2. 返回主菜单，点开工具->导出星球脚本，导出行星系统
3. 在导出的目录(通常是SE目录/export)下找到导出的系统文件
4. 转到生成器的目录，用命令行打开该目录，输入以下命令或直接把文件拖入主程序：
```
infogen <Filename> <args...>
```
其中infogen为主程序名称，Filename为文件名，args为附加参数(不指定文件直接运行即可查看)

----------
*正因为有些事不能挽回也不能改变，人间才会有情感。 ——纳西妲*
