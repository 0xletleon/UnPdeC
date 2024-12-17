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
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>
#include <sstream>
#include <exception>

/* 数据类型 */

// 当前xor文件信息
struct TNowXor {
	std::string Name; // xor名称
	long Size; // xor大小
	std::string Path; // xor路径
	std::string JsonPath; // XorJson路径
};

// GetByteOfXor() 返回的数据结构 
struct GetOffsetStr {
	uint64_t Size; // 实际获取到的块大小
	std::vector<uint8_t> Byte; // 获取到的字节数据
};

// 从.xor中读取文件信息
struct BlockInfo {
	bool isValid = false; // 块是否有效
	std::string type; // "1" for file, "2" for folder
	std::string name; // 文件名
	std::string encodedOffset; // 编码后的偏移值
	std::string decodedOffset; // 解码后的偏移值
	std::string size; // 数据块大小
	std::string blockOffset; // 块的偏移值
	std::string blockEncodedOffset; // 块的编码后的偏移值
};

// 解码保存时的数据块信息
struct MataSaveInfo {
	std::string type; // "1" for file, "2" for folder
	std::string name; // 文件名
	uint64_t offset; // 文件偏移值
	uint64_t size; // 文件大小
	std::string directoryPath; // 文件所在文件夹路径
};