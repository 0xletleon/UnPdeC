#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>

//using std::string;
//using std::vector;
//using std::cout;
//using std::cin;
//using std::dec;
//using std::hex;
//using std::ios;
//using std::ifstream;
//using std::filesystem::path;
//using std::cerr;
//using std::regex;
//using std::all_of;
//using std::isdigit;
//using std::isalpha;
using std::min;

/* 数据类型 */

// 当前Pde文件信息
struct TNowPde {
	std::string Name; // Pde名称
	long Size; // Pde大小
	std::string Path; // Pde路径
};

// 文件夹信息
struct DirStr {
	std::filesystem::path UpDir; // 上级目录
	std::filesystem::path NowDir; // 当前目录
};

// 文件偏移信息
struct HexOffsetInfo {
	uint8_t Type; // 1 文件, 2 目录
	std::string Name; // 文件名或目录名
	uint64_t PdeOffset; // 在PDE文件中的实际偏移值
	uint64_t Size; // 大小
	uint64_t OriginalOffset; // 原始偏移值
	size_t PatternIndex; // 用于存储模式索引
};

// GetByteOfPde() 返回的数据结构 
struct GetOffsetStr {
	uint64_t Size; // 实际获取到的块大小
	std::vector<uint8_t> Byte; // 获取到的字节数据
};