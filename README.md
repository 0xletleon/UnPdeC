![UnPdeC.png](README/UnPdeC.png)

# 🎮 UnPdeC

    此项目是Unpde(c#)的替代项项目
    使用 腾讯元宝，Kimi, Claude 辅助开发
    使用新的方法来导出全部文件！
    为了学习编程和加解密算法而研究
    请勿用于任何商业目的！

## 🦋 依赖

    nlohmann.json" version="3.11.2"
    FastXor.exe

## 🔨 Build

    C++标准 -> C++20
    VS2022
    Win64平台

## 🔧 测试程序

[UnPdeC.exe](Release/UnPdeC.exe)

[FastXOR.exe](Release/FastXOR.exe)

## 🚀 使用

    0: .pde文件需要和 FastOR.exe与UnPdeC.exe在同一目录下
    1：FastXOR.exe 异或.pde文件
    2：UnPdec.exe 即可解包.xor文件 

## 🐛 DEBUG辅助工具

![FastXOR.png](README/FastXOR.png)

### 本方案内项目

#### [FastXOR C++版(本方案内项目)](Release/FastXOR.exe)

#### [FastXOR Html版](Tools/FastXor.html)

#### [PDE偏移值计算器](Tools/OffsetCalc.html)

#### [文件偏移值模糊计算器](Tools/XorShiftOffset.html)

#### [Blender PDE Mesh 导入插件](Tools/import_mesh.py)
    目前只能导入顶点，Blender 4.1'
    严禁用于商业目的，仅为学习交流！！！

### 其他仓库

#### [PDEVerify](https://letleon.coding.net/public/3d/PDEVerify/git/files)

#### [PDEVerifyGUI](https://letleon.coding.net/public/3d/PDEVerifyGUI/git/files)

### 🧊🧸 探索 Mesh文件

    经过与AI们亲切友好交流之后发现.mesh文件似乎可以手动重建！
    使用 Blender PDE Mesh 导入插件可以导入部分文件
    当然这也仅仅只是个测试！

![mesh1.png](README/mesh1.png)

    👆 导入之后只有顶点，没有边和面，上面是暴力F之后自动建立的面
    可以看出这是训练关中胖子木板假人模型

    0X31处的4个字节是模型数据块总大小
        10A00000 -> 模型数据块总大小

    mesh中每0X34为一个顶点的信息(还需要继续探索)
        D4DEC6BD C151723E A579B13D -> 顶点 X Y Z
        00000000 003C0000 210000BC -> 未知
        00000000 00BC2180 00000000 -> 未知
        ED38CA3A 3D37BF34 00380038 -> 未知
        FFFFFFFF -> 结束标记？

![mesh2.png](README/mesh2.png)

    这是火焰兵？那个喜欢烤肉的男人？🔥

![mesh3.png](README/mesh3.png)

    这是根据顶点信息手动创建边和面，在加上贴图后的金币！

## 🐼🐻‍❄️🐻 线索

### 根目录

    0X1000 - 0X1FFF
    使用 FastXor.html 即可验证

### 文件信息

![fileinfo.jpg](README/fileinfo.jpg)

    类型 (黄色)
        01 = 文件 , 02 = 目录

    文件名 (绿色)
        类型之后紧跟的就是文件名

    偏移值 (蓝色)
        相对位置在 0X78-0X7B
        这个偏移值可以使用OffsetCalc.html计算处实际的偏移值
        这个算法很有意思

    大小 (紫色)
        相对位置在 0X7C-0X7F

    文件信息块特征1 (橙色)
        解密后可以观察到每个在目录中的"文件"都有一个标识?
        相对位置在 0X74-0X77 -> C7D5CC01(每个文件中都不一样)

    文件信息块特征2 (橙框)
        0X60 - 0X6F
        这个区域逻辑上是给名称用的
        但是大多数写不到这一行
        所以绝大部分(未验证)是00

### 奇怪的地方

#### 奇怪的名字

![warn.jpg](README/warn.jpg)

    正常来说文件名后面是00的话文件名就结束了
    有几处会出现00后面还有字符串的情况

![warn2.jpg](README/warn2.jpg)

    在某更新补丁内找到了相同的情况
    看样子应该是被复用了
    程序只读到00就结束了
    所以不影响

#### 不匹配的数据大小

![warn3.jpg](README/warn3.jpg)

    可以看到 team 文件夹描述中写的是0x4000
    但是实际上却只有 0x100 大小
    再往后的内容是某个DDS文件数据的空间

    也就是说依赖描述中的信息有些不可靠了！
    或者说这些错误的信息是被抛弃的？

### 文件验证

![pdeverify](README/pdeverify.jpg)

[PDEVerify](https://letleon.coding.net/public/3d/PDEVerify/git/files)

[PDEVerifyGUI](https://letleon.coding.net/public/3d/PDEVerifyGUI/git/files)

## 🖼️🎵 资源预览工具

### DDS/TGA预览工具

[TacentView]([Tools/DDSView.html](https://github.com/bluescan/tacentview))

### FSB音频提取工具

[FSB Extractor](http://aezay.dk/aezay/fsbextractor/)

    有些音乐听起来可能怪怪的，
    可以用音乐转换工具将mp3还转换成mp3格式就可以正常听了。
    比如千千静听,foobar2000都内置转换器！
