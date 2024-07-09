// Common.h
#ifndef COMMON_H
#define COMMON_H

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

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;
using std::dec;
using std::hex;
using std::ios;
using std::ifstream;
using std::filesystem::path;
using std::cerr;
using std::regex;
using std::all_of;
using std::isdigit;
using std::isalpha;

/* 数据类型 */

/// <summary>
/// 当前Pde文件信息
/// </summary>
struct TNowPde {
	string Name; // Pde名称
	long Size; // Pde大小
	string Path; // Pde路径
};

/// <summary>
/// 文件夹信息
/// </summary>
struct DirStr {
	std::filesystem::path UpDir; // 上级目录
	std::filesystem::path NowDir; // 当前目录
};

/// <summary>
/// 文件偏移信息
/// </summary>
struct HexOffsetInfo {
	uint8_t Type; // 1 文件, 2 目录
	string Name;	// 文件名或目录名
	uint32_t PdeOffset; // 在PDE文件中的实际偏移值
	uint32_t Size; // 大小
	uint32_t OriginalOffset; // 原始偏移值
};

/// <summary>
/// GetByteOfPde() 返回的数据结构 
/// </summary>
struct GetOffsetStr {
	uint32_t Size;  // 实际获取到的块大小
	vector<uint8_t> Byte;  // 获取到的字节数据
};
#endif // COMMON_H