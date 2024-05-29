// Common.h
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::cin;
using std::dec;
using std::hex;

/// <summary>
/// 当前Pde文件信息
/// </summary>
struct TNowPde {
	// Pde名称
	string Name;
	// Pde大小
	long Size;
};

#endif // COMMON_H