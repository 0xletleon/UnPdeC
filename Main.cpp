#include "Main.h"

int main(int argc, char* argv[]) {
	// 设置控制台标题和图标
	SetConsoleTitle(TEXT("UnPdeC Version: 0.0.2.1 仅限学习严禁商用"));
	HWND hwnd = GetConsoleWindow();
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
	if (hIcon) {
		SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
	}

	// 显示程序信息和使用说明
	std::cout << "UnPdeC\n"
		<< "By: letleon\n"
		<< "Version: 0.0.2.1\n"
		<< "解压.xor文件 Unpack .xor files\n\n"
		<< "步骤 step:\n"
		<< "	1.用 FastXOR.exe 对 .pde 文件执行异或操作。\n	  Use FastXOR.exe to perform an XOR operation on the.pde file.\n"
		<< "	2.运行 UnPdeC.exe 解包生成的 .xor 文件。\n	  Run UnPdeC.exe to unpack the generated.xor file.\n\n"
		<< "选项 Options:\n"
		<< "	-c  不执行二次解压,同时保留文件的前8个字节\n	    Do not perform secondary decompression and retain the first 8 bytes of the file.\n\n";


	// 检查命令行参数
	if (argc == 2 && std::string(argv[1]) == "-c") {
		GV::CacheMode = true;
		std::cout << " ！Cache mode enabled\n\n";
	} else if (argc != 1) {
		std::cout << "！Invalid argument\n";
		return 1;
	}

	// 搜索并解密所有.xor文件
	std::vector<TNowXor> xorList = Tools::FindXorFile();

	// 遍历所有文件.xor文件
	for (const TNowXor& ThisXor : xorList) {
		std::cout << " >> Decoding: " << ThisXor.Name << "\n";

		GV::NowXor = ThisXor;//将当前处理的.xor信息传递给GV

		Tools::Init();//初始化

		std::cout << "JsonPath: " << ThisXor.JsonPath << "\n";

		// 偏移映射表
		nlohmann::json offsetMaps;

		// 判断 ThisXor.JsonPath 是否存在
		if (std::filesystem::exists(ThisXor.JsonPath)) {
			std::cout << "Json found!\n";
			// 读取ThisXor.JsonPath
			std::ifstream ifs(ThisXor.JsonPath);
			offsetMaps = nlohmann::json::parse(ifs);
			ifs.close();
			std::cout << "Json loaded!\n";
		} else {
			std::cout << ThisXor.JsonPath << "Json not found!\n";
			offsetMaps = Mata::ReadMatakForXorFile(GV::NowXor.Path);//读取.xor中的文件/夹信息
		}

		Mata::ExtractMateAndDecode(offsetMaps);//解码文件/夹到磁盘
	}

	std::cout << " √ Decoding complete!\n";
	std::cin.get();
	return 0;
}