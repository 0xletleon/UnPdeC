#include "Main.h"
#include <filesystem>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "resource.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

// 从.xor中读取文件信息
struct FileInfo {
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
struct SaveInfo {
	std::string type; // "1" for file, "2" for folder
	std::string name; // 文件名
	uint64_t offset; // 文件偏移值
	uint64_t size; // 文件大小
	std::string directoryPath; // 文件所在文件夹路径
};

// HEX -> "0x1234"
std::string toHexString(uint64_t value) {
	std::stringstream stream;
	stream << "0x" << std::uppercase << std::hex << value;
	return stream.str();
}

/// <summary>
/// 编码后的偏移值 -> 中的实际偏移值
/// </summary>
/// <param name="originalOffset">编码后的偏移值</param>
/// <returns>pde中的实际偏移值</returns>
uint64_t originalToPdeOffset(uint64_t originalOffset) {
	return ((originalOffset >> 10) + originalOffset + 1) << 12;
}

/// <summary>
/// pde中的实际偏移值 -> 编码后的偏移值
/// </summary>
/// <param name="pdeOffset">pde中的实际偏移值</param>
/// <returns>编码后的偏移值</returns>
uint64_t pdeToOriginalOffset(uint64_t pdeOffset) {
	return (pdeOffset >> 12) * 1024 / 1025;
}

/// <summary>
/// 验证名称
/// </summary>
/// <param name="name">名称</param>
/// <returns>是否合法</returns>
bool isValidFileName(const std::string& name) {
	if (name.empty() || name.back() == ' ' || name.back() == '.') {
		return false;
	}
	return std::all_of(name.begin(), name.end(), [](char ch) {
		return (ch > 0x20 && ch < 0x7F) && ch != '\"' && ch != '*' && ch != '/' &&
			ch != ':' && ch != '<' && ch != '>' && ch != '?' && ch != '\\' && ch != '|';
		});
}

/// <summary>
/// 验证并提取文件信息
/// </summary>
/// <param name="block">0x80大小的数据库</param>
/// <param name="fileSize">.xor文件总大小</param>
/// <returns>提取到的文件信息</returns>
FileInfo extractFileInfo(const uint8_t* block, uint64_t fileSize) {
	// 提取到的文件信息
	FileInfo fileInfo{};

	// 判断文件名是否合法
	if (block[0] != 0x01 && block[0] != 0x02) {
		return fileInfo;
	}

	// 判断文件名是否合法
	std::string name{ &block[1], std::find(&block[1], &block[0x50], '\0') };
	if (!isValidFileName(name)) {
		return fileInfo;
	}

	// 判断此区域是否位0x0
	if (!std::all_of(block + 0x50, block + 0x70, [](uint8_t c) { return c == 0x00; })) {
		return fileInfo;
	}

	// 获取并判断编码后的偏移值是否合法
	uint64_t originalOffset = *reinterpret_cast<const uint32_t*>(&block[0x78]);
	if (originalOffset < 1) {
		return fileInfo;
	}

	// 读取并判断文件大小是否合法
	uint64_t size = *reinterpret_cast<const uint32_t*>(&block[0x7c]);
	if (size >= fileSize || size == 0) {
		return fileInfo;
	}

	// 拼接信息
	fileInfo = {
		true,
		(block[0] == 0x01) ? "1" : "2",
		name,
		toHexString(originalOffset),
		toHexString(originalToPdeOffset(originalOffset)),
		toHexString(size),
		"",
		""
	};

	// 返回文件/夹信息
	return fileInfo;
}

/// <summary>
/// 读取.xor中的所有元数据
/// </summary>
/// <param name="filePath">.xor文件路径</param>
/// <returns>所有文件/夹的元数据json</returns>
json ReadMatakForXorFile(const fs::path& filePath) {
	if (!fs::exists(filePath)) {
		throw std::runtime_error("File does not exist: " + filePath.string());
	}

	// 读取文件
	std::ifstream file(filePath, std::ios::binary);
	if (!file) {
		throw std::runtime_error("Unable to open file: " + filePath.string());
	}

	file.seekg(0, std::ios::end);// 获取文件大小
	uint64_t fileSize = file.tellg();// .xor文件大小
	file.seekg(0x1000, std::ios::beg);// 跳过文件头 0x1000是根目录

	const uint64_t blockSize = 0x80;// 块大小
	std::vector<uint8_t> buffer(blockSize);// 缓冲区
	json maps;// 存储目录与文件映射信息
	json list;// 存储目录与文件元信息

	uint64_t offsetIndex = 0x1000;// 跳过的偏移量
	while (file.read(reinterpret_cast<char*>(buffer.data()), blockSize)) {
		// 读取并验证块
		FileInfo fileInfo = extractFileInfo(buffer.data(), fileSize);

		if (fileInfo.isValid) {
			// 获取当前偏移值对应的编码偏移值
			auto blockEncodedOffset = toHexString(pdeToOriginalOffset(offsetIndex));
			auto blockOffsetStr = toHexString(offsetIndex);
			// 存储目录与文件映射信息
			maps[toHexString(pdeToOriginalOffset(offsetIndex))].push_back(fileInfo.encodedOffset);
			// 存储目录与文件元信息
			list[fileInfo.encodedOffset] = {
				{"type", fileInfo.type},
				{"name", fileInfo.name},
				{"encodedOffset", fileInfo.encodedOffset},
				{"decodedOffset", fileInfo.decodedOffset},
				{"size", fileInfo.size},
				{"blockOffset", blockOffsetStr},
				{"blockEncodedOffset", blockEncodedOffset}
			};
		}
		// 下一个块
		offsetIndex += blockSize;
	}

	// 合并JSON
	json output = {
		{"maps", maps},
		{"list", list}
	};

	// 保存元信息到磁盘
	std::string outputPath = (GV::ExeDir / (GV::NowPde.Name + ".json")).string();
	std::ofstream outFile(outputPath);
	outFile << output.dump(4);
	outFile.close();

	std::cout << "JSON file has been saved to: " << outputPath << "\n";

	// 返回JSON
	return output;
}

/// <summary>
/// 解码并保存文件
/// </summary>
/// <param name="saveInfo">文件信息</param>
void DecodeAndSaveFile(const SaveInfo& saveInfo) {
	// 文件
	if (saveInfo.type == "1") {
		// 读取文件
		GetOffsetStr tempFileByte = PdeTool::GetByteOfPde(saveInfo.offset, saveInfo.size);
		// 检查文件大小
		if (tempFileByte.Size != saveInfo.size) return;

		// Ensure directory exists
		fs::path dirPath = saveInfo.directoryPath;
		if (!fs::exists(dirPath)) {
			fs::create_directories(dirPath);
		}

		// Check if the file name contains ".cache" to determine if it needs secondary decryption
		bool hasCache = saveInfo.name.find(".cache") != std::string::npos;

		// 无需二次解码
		if (!hasCache) {
			// Save file
			fs::path filePath = dirPath / saveInfo.name;
			if (!fs::exists(filePath)) {
				std::ofstream outFile(filePath, std::ios::binary);
				outFile.write(reinterpret_cast<const char*>(tempFileByte.Byte.data()), tempFileByte.Byte.size());
				outFile.close();
			}
		} else {
			// Secondary decryption
			std::vector<uint8_t> decryption2;
			std::string fixedName = saveInfo.name;
			try {
				// 二次解码
				decryption2 = FinalUnpack::PreDecrypt(tempFileByte.Byte, fixedName);
				fixedName = saveInfo.name.substr(0, saveInfo.name.find(".cache"));
			} catch (const std::exception&) {
				std::cout << "Secondary decryption failed: " << saveInfo.name << "\n";
				decryption2 = tempFileByte.Byte;
			}

			// Save file
			fs::path filePath2 = dirPath / fixedName;
			if (!fs::exists(filePath2)) {
				std::ofstream outFile(filePath2, std::ios::binary);
				outFile.write(reinterpret_cast<const char*>(decryption2.data()), decryption2.size());
				outFile.close();
			}
		}
	} else if (saveInfo.type == "2") {
		// Create directory
		fs::path parentPath = saveInfo.directoryPath;
		fs::path dirName = saveInfo.name;
		fs::path dirPath = parentPath / dirName;
		if (!fs::exists(dirPath)) {
			fs::create_directories(dirPath);
			std::cout << "Directory created: " << dirPath << "\n";
		}
	} else {
		std::cout << "Unknown type: " << saveInfo.name << "\n";
	}
}

/// <summary>
/// 解码文件/夹到磁盘
/// </summary>
/// <param name="offsetMaps">元信息JSON</param>
void UnPackForJson(const json& offsetMaps) {
	// 遍历所有Map映射信息
	for (const auto& [key, values] : offsetMaps.at("maps").items()) {
		if (values.empty()) {
			std::cout << "Empty map key: " << key << "\n";
			continue;
		}

		////////////////////////读取并拼接目录////////////////////////

		// 当前路径向量
		std::vector<std::string> currentDirPath;

		// 获取编码后的目录值 key -> "0x123"
		uint64_t blockOffsetHex = std::stoull(key.substr(2), nullptr, 16);

		// 判断是否是根目录 0 是根目录(可以使用FastXor.html验证)
		bool isRootDir = blockOffsetHex == 0;
		if (isRootDir) {
			std::cout << "Root directory\n";
		}

		// 非根目录 与 List下存在编码后的key
		if (!isRootDir && offsetMaps.at("list").contains(key)) {
			// 获取当前目录对象
			json currentDirObj = offsetMaps.at("list")[key];

			// 判断是否为目录
			if (currentDirObj["type"] != "2") {
				std::cout << "Error! Not a directory\n";
				continue;
			}

			// 获取当前目录名称 -> currentDirPath向量
			currentDirPath.push_back(currentDirObj["name"]);
			// 获取16进制的编码后的偏移值
			uint64_t upDirBlockOOffsetHex = std::stoull(currentDirObj["blockEncodedOffset"].get<std::string>(), nullptr, 16);

			// 如果 > 0 则表示还有上级目录
			while (upDirBlockOOffsetHex > 0) {
				// 获取上级目录的编码偏移值
				std::string upOffset = currentDirObj["blockEncodedOffset"];

				// 判断是否存在
				if (offsetMaps.at("list").contains(upOffset)) {
					// 存在
					// 获取上级目录对象
					currentDirObj = offsetMaps.at("list")[upOffset];
					// 获取上级目录名称 -> currentDirPath向量
					currentDirPath.push_back(currentDirObj["name"]);
					// 获取上级目录的编码偏移值
					upDirBlockOOffsetHex = std::stoull(currentDirObj["blockEncodedOffset"].get<std::string>(), nullptr, 16);
				} else {
					// 不存在则放到 Other 目录下
					currentDirPath.push_back("Other");
					break;
				}
			}
		} else {
			// 非根目录
			if (!isRootDir) {
				// 编码偏移值 -> currentDirPath向量
				currentDirPath.push_back(key);
				// 放入Other目录
				currentDirPath.push_back("Other");
			}
		}

		// 当前正在处理的.xor文件名(无后缀) -> currentDirPath向量
		currentDirPath.push_back(GV::NowPde.Name);
		if (currentDirPath.size() > 1) {
			// 反转
			std::reverse(currentDirPath.begin(), currentDirPath.end());
		}

		// 拼接后的路径
		std::string dirTempPath;
		for (const auto& path : currentDirPath) {
			dirTempPath += path + "\\";
		}

		// 拼接后的绝对路径
		std::string filePath = (GV::ExeDir / dirTempPath).string();
		std::cout << "Directory path: " << filePath << "\n";

		////////////////////////遍历解码保存目录下的文件////////////////////////

		// 遍历目录下的文件
		for (const auto& value : values) {
			// 判断是否包含
			if (offsetMaps.at("list").contains(value)) {
				// 获取信息
				json info = offsetMaps.at("list")[value];
				// 提取信息
				SaveInfo saveInfo{
					info["type"],
					info["name"],
					std::stoull(info["decodedOffset"].get<std::string>(), nullptr, 16),
					std::stoull(info["size"].get<std::string>(), nullptr, 16),
					filePath
				};
				// 解码并保存文件
				DecodeAndSaveFile(saveInfo);
			}
		}
	}
}

int main() {
	// 设置控制台标题和图标
	SetConsoleTitle(TEXT("UnPdeC Version: 0.0.2.0"));
	HWND hwnd = GetConsoleWindow();
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
	if (hIcon) {
		SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hIcon));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hIcon));
	}

	// 启用 ANSI 转义序列
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	// 显示程序信息和使用说明
	std::cout << "UnPdeC\n By:letleon\n Version: 0.0.2.0\n Desc: UnPack Pde file\n\n";
	std::cout << "Usage:\n Firstly, use FastXOR.exe to XOR the. pde file\n Then, execute UnPdeC.exe to unpack .xor file\n\n";

	// 搜索并解密所有.xor文件
	std::vector<TNowPde> pdeArray = FindPde::Get();
	// 遍历所有文件.xor文件
	for (const TNowPde& pde : pdeArray) {
		std::cout << "Decoding: " << pde.Name << "\n";

		GV::NowPde = pde;//将当前处理的.xor信息传递给GV

		PdeTool::Init();//初始化

		json offsetMaps = ReadMatakForXorFile(GV::NowPde.Path);//读取.xor中的文件/夹信息

		UnPackForJson(offsetMaps);//解码文件/夹到磁盘
	}

	std::cout << "Decoding complete!\n";
	std::cin.get();
	return 0;
}