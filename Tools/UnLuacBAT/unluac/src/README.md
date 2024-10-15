# 感谢
    感谢 AeonLucid 的开源项目
    项目地址 https://github.com/AeonLucid/AvatarStarCBT

# 将java项目编译成独立的可执行文件

## 下载 Oracle GraalVM
    https://www.graalvm.org/downloads/

    设置环境变量
    这个和设置JDK的环境变量是一样的，
    也就是说Oracle GraalVM可以编译java项目为 (.jar)
    以及可以编译成独立的可执行文件 (.exe)

## 编译为.jar
    运行 build_jar.bat.bat 即可得到 unluac.jar

## 编译为.exe
    运行 build_native.bat 将会生成一个 unluac.exe
    将 unluac.exe 重命名为 UnLuacGVM.exe

## 参与编译
    将UnLuacGVM.exe放置到UnluacBAT项目目录下
    即可在编译时使用它

## 与 UnluacBAT 一同使用
    放置到 UnluacBAT 同目录下即可运行 UnLuacBAT
    UnluacBAT 是一个前置程序用来向 UnLuac 传参

## 直接使用
    如果你不想安装JDK JRE那么现在可以直接使用
    UnluacGVM.exe --rawstring --lua_version 5.2-alpha test.luac > test.luac
    程序名 参数1 参数2 输入文件名.luc 输出符号 输出文件名.lua
    
    这样可以输出 GB2312 编码的lua文件
    使用vscode 以 GB2312 编码打开，然后在保存为UTF-8编码即可正常显示中文

    部分文件默认就是UTF-8编码！
