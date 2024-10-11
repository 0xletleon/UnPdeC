#include "Main.h"

int main() {
	// 设置控制台标题和图标
	SetConsoleTitle(TEXT("UnPdeC Version: 0.0.2.0 仅限学习严禁商用"));
	HWND hwnd = GetConsoleWindow();
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
	if (hIcon) {
		SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
	}

	// 显示程序信息和使用说明
	std::cout << "UnPdeC\n	By:letleon\n	Version: 0.0.2.0\n	Desc: UnPack Pde file\n\n";
	std::cout << "Usage:\n	Firstly, use FastXOR.exe to XOR the. pde file\n	Then, execute UnPdeC.exe to unpack .xor file\n\n";

	// 搜索并解密所有.xor文件
	std::vector<TNowXor> xorList= Tools::FindXorFile();

	// 遍历所有文件.xor文件
	for (const TNowXor& ThisXor : xorList) {
		std::cout << " >> Decoding: " << ThisXor.Name << "\n";

		GV::NowXor = ThisXor;//将当前处理的.xor信息传递给GV

		Tools::Init();//初始化

		json offsetMaps = Mata::ReadMatakForXorFile(GV::NowXor.Path);//读取.xor中的文件/夹信息

		Mata::ExtractMateAndDecode(offsetMaps);//解码文件/夹到磁盘
	}

	std::cout << " √ Decoding complete!\n";
	std::cin.get();
	return 0;
}