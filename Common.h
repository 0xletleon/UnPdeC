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
/// ��ǰPde�ļ���Ϣ
/// </summary>
struct TNowPde {
	// Pde����
	string Name;
	// Pde��С
	long Size;
};

#endif // COMMON_H