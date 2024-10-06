#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "Resource.h"

static std::vector<uint8_t> PDEKEY; // PDEKEY

/// <summary>
/// 异或解密文件
/// </summary>
/// <param name="FileVec">文件向量指针</param>
/// <returns>解密后的数据</returns>
static std::vector<uint8_t> XorFile(const std::vector<uint8_t>& FileVec) {
	std::vector<uint8_t> Result(FileVec.size()); // Result 向量
	constexpr size_t KeyLength = 0x1000; // PDEKEY的固定长度

	// 进行异或操作
	for (size_t i = 0; i < FileVec.size(); ++i) {
		size_t KeyIndex = i % KeyLength; // 使用模运算确保 KeyIndex 循环在 0 到 KeyLength 之间
		Result[i] = PDEKEY[KeyIndex] ^ FileVec[i];
	}

	return Result; // 完成解密数据块
}

// 获取PDEKEY
static std::vector<uint8_t> GetPdeKey() {
	std::vector<uint8_t> KeyByte(0x1000, 0); // 初始化为0

	// 逻辑从 汇编 0x00A608E0 处获得
	uint32_t EAX = 0x42574954;
	// 计算KEY
	for (size_t i = 0; i < 0x1000; ++i) {
		EAX *= 0x7FCF;
		uint32_t CL = (EAX >> 24) ^ ((EAX >> 16) & 0xFF) ^ ((EAX >> 8) & 0xFF) ^ (EAX & 0xFF);
		KeyByte[i] = static_cast<uint8_t>(CL);
	}

	std::cout << "√PDEKEY Get Success\n\n";
	return KeyByte;
}

/// <summary>
/// 处理单个文件
/// </summary>
/// <param name="filePath">文件地址</param>
static void processFile(const std::filesystem::path& filePath) {
	// 跳过.xor文件
	if (filePath.extension() == ".xor") {
		std::cout << "!!! Skipping .xor File: " << filePath.filename() << "\n";
		return;
	}

	// 检查文件是否存在
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "! File does not exist: " << filePath << "\n";
		return;
	}

	// 读取文件
	std::ifstream file(filePath, std::ios::binary);
	if (!file) {
		std::cerr << "! Failed to open file: " << filePath << "\n";
		return;
	}

	// 读取文件内容
	std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
	std::cout << ">> File: " << filePath.filename() << " Size: " << buffer.size() << " bytes\n";

	// 异或操作
	std::vector<uint8_t> Result = XorFile(buffer);

	// 构造输出文件路径
	std::filesystem::path outputFilePath = filePath;
	outputFilePath.replace_extension(outputFilePath.extension().string() + ".xor");

	// 写入文件
	std::ofstream outputFile(outputFilePath, std::ios::binary);
	if (!outputFile) {
		std::cerr << "! Failed to open output file: " << outputFilePath << std::endl;
		return;
	}
	outputFile.write(reinterpret_cast<char*>(Result.data()), Result.size());

	std::cout << "<< Output: " << outputFilePath.filename() << "\n";
}

/// <summary>
/// 递归遍历目录
/// </summary>
/// <param name="dirPath">文件夹地址</param>
static void processDirectory(const std::filesystem::path& dirPath) {
	std::cout << ">> Directory: " << dirPath << "\n";

	// 首先处理当前目录下的所有文件
	for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
		if (entry.is_regular_file()) {
			processFile(entry.path());
		}
	}

	// 然后递归处理子目录
	for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
		if (entry.is_directory() && !entry.path().filename().empty() && entry.path().filename() != "." && entry.path().filename() != "..") {
			// 递归处理子目录
			processDirectory(entry.path());
		}
	}
}

/// <summary>
/// Main
/// </summary>
/// <param name="argc">参数数量</param>
/// <param name="argv">参数数组</param>
int main(int argc, char* argv[]) {
	// 设置控制台 标题和图标
	SetConsoleTitle(TEXT("FastXOR")); // 设置控制台窗口的标题
	HWND hwnd = GetConsoleWindow(); // 获取控制台窗口的句柄
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON)); // 加载图标
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon); // 设置控制台窗口的图标
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// 显示使用说明
	std::cout << "FastXOR\n By:letleon\n Version: 0.0.1.0\n Desc: FAST XOR PDE BIN FILE\n\n";
	std::cout << "Usage:\n FastXOR.exe <file_path>\n Or drag and drop files or a directory onto the FastXOR.exe \n\n";

	// 检查命令行参数
	if (argc < 2) {
		std::cin.get();
		return 1;
	}

	PDEKEY = GetPdeKey(); // 获取PDEKEY

	// 遍历命令行参数
	for (int i = 1; i < argc; ++i) {
		std::filesystem::path filePath = argv[i];
		if (std::filesystem::is_directory(filePath)) {
			processDirectory(filePath); // 文件夹递归处理
		} else if (std::filesystem::is_regular_file(filePath)) {
			processFile(filePath); // 处理单个文件
		} else {
			std::cerr << "! Invalid Path: " << filePath << "\n";
		}
	}
	std::cout << "\n√Processing Completed...";
	std::cin.get();
	return 0;
}