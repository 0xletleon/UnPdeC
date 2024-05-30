// Common.h
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <fstream>
#include <regex>

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

/// <summary>
/// ��ǰPde�ļ���Ϣ
/// </summary>
struct TNowPde {
	string Name; // Pde����
	long Size; // Pde��С
	string Path; // Pde·��
};

/// <summary>
/// �ļ�����Ϣ
/// </summary>
struct DirStr {
	string UpDir; // �ϼ�Ŀ¼
	string NowDir; // ��ǰĿ¼
};

/// <summary>
/// �ļ�ƫ����Ϣ
/// </summary>
struct HexOffsetInfo {
	uint8_t Type; // 1 �ļ�, 2 Ŀ¼
	string Name;	// �ļ�����Ŀ¼��
	uint32_t Offset; // ��PDE�ļ��е�ʵ��ƫ��ֵ
	uint32_t Size; // ��С
	uint32_t OOffset; // ԭʼƫ��ֵ
};

/// <summary>
/// GetByteOfPde() ���ص����ݽṹ 
/// </summary>
struct GetOffsetStr {
	uint32_t Size;  // ʵ�ʻ�ȡ���Ŀ��С
	vector<uint8_t> Byte;  // ��ȡ�����ֽ�����
};

#endif // COMMON_H