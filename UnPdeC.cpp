// UnPdeC.cpp
#include "UnPdeC.h"
#include "ExtraSearch.h"
#include <filesystem>

int main() {
	cout << "Hello CMake." << endl;

	// try {
	// 	//文件名 155
	// 	string finaName = "loadingmap_level_train.dds.cache";
	// 	// 获取程序所在目录并拼接文件名
	// 	std::filesystem::path file_path = std::filesystem::current_path();

	// 	// 打开文件
	// 	std::ifstream file(file_path / "TestFile" / finaName, std::ios::binary);
	// 	if (!file) {
	// 		std::cerr << "无法打开文件: " << file_path / "TestFile" / finaName << std::endl;
	// 		return 1;
	// 	}

	// 	// 读取文件内容到字节向量中
	// 	std::vector<uint8_t> encrypted_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	// 	// 关闭文件
	// 	file.close();

	// 	std::vector<uint8_t> xaa = FinalUnpack::PreDecrypt(encrypted_data, finaName);
	// 	cout << "xaa size: " << xaa.size() << endl;
	// 	// 将 xaa 写入程序所在目录
	// 	std::ofstream out("TestFile/" + finaName + ".bin", std::ios::binary);
	// 	out.write((char*)&xaa[0], xaa.size());
	// 	out.close();
	// } catch (const std::filesystem::filesystem_error& e) {
	// 	std::cerr << "文件系统错误: " << e.what() << std::endl;
	// 	return 1;
	// } catch (const std::exception& e) {
	// 	std::cerr << "异常: " << e.what() << std::endl;
	// 	return 1;
	// }

	// 查找当前目录下所有的.pde文件
	vector<TNowPde> PdeArr = FindPde::Get();
	//遍历PdeArr进行解密
	for (TNowPde pde : PdeArr) {
		cout << " ！正在解码：" << pde.Name << endl;
		// 设置当前解密的文件名
		GV::NowPde = pde;

		// 初始化PDETool
		PdeTool::Init();

		// 创建目录结构
		// 由于是根目录，所以由pde文件名来命名根目录文件夹名
		DirStr TryDir = { pde.Name , pde.Name };

		// 从根目录开始解密
		// 可得到大部分文件和目录
		Unpack::Try(0x1000, 0x1000, TryDir, false);

		/*TryDir = { pde.Name, pde.Name + "/Other" };
		Unpack::Try(0x8f6000, 0x1000, TryDir, false);
		Unpack::Try(0x953000, 0x1000, TryDir, false);*/
	}

	// 保存日志到文件
	OffsetLog::SaveToFile();

	//OffsetLog::WriteToGV();

	//ExtraSearch::SearchAndDecrypt();

	cout << "解密完成！" << endl;
	return 0;
}


//#include <iostream>
//#include <vector>
//#include <string>
//#include <filesystem>
//#include <fstream>
//#include <algorithm>
//#include <thread>
//#include <mutex>
//#include <Windows.h>
//#include <nlohmann/json.hpp>
//#include <iomanip>
//#include <sstream>
//
//namespace fs = std::filesystem;
//using json = nlohmann::json;
//
//struct Signature {
//	std::string name;
//	std::vector<uint8_t> bytes;
//};
//
////std::vector<Signature> signatures = {
////	{".anim", {0x1F, 0xD4, 0xE3, 0x7B, 0x68, 0xD9, 0xE1, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".lua/.occ - FC", {0x1E, 0xDB, 0xE3, 0x7B, 0x68, 0xD9, 0xE3, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".lua - AS",{0x04, 0x9D, 0x96, 0x1A, 0x3B, 0xD9, 0xE3, 0xCB, 0xF9, 0x10, 0xCB, 0x67}},
////	{".tag/.dds", {0x1E, 0xD3, 0xE3, 0x7B, 0x68, 0xD9, 0xE3, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{"noise3d.dds", {0x1D, 0xD3, 0xE3, 0x7B, 0x68, 0xD9, 0xE3, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".dcl", {0x1F, 0xDD, 0xE3, 0x7B, 0x68, 0xD9, 0xE0, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".mesh", {0x1F, 0xD0, 0xE3, 0x7B, 0x68, 0xD9, 0xEA, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".physx", {0x1F, 0xD6, 0xE3, 0x7B, 0x68, 0xD9, 0xE7, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".pd9", {0x1D, 0xD5, 0xE3, 0x7B, 0x68, 0xD9, 0xE1, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".vd9", {0x1E, 0xD5, 0xE3, 0x7B, 0x68, 0xD9, 0xE1, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".skel", {0x1F, 0xD7, 0xE3, 0x7B, 0x68, 0xD9, 0xE1, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".spr", {0x1F, 0xDC, 0xE3, 0x7B, 0x68, 0xD9, 0xE2, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".vfx", {0x1F, 0xD2, 0xE3, 0x7B, 0x68, 0xD9, 0xE6, 0xCF, 0xFD, 0x14, 0xC3, 0x67}},
////	{".fsb", {0x59, 0x82, 0xA1, 0x4F}},
////	{".swf", {0x5C, 0x86, 0xB0, 0x71}},
////	{".ttf", {0x1F, 0xD0, 0xE3, 0x7B, 0x6A, 0xCB, 0xE3, 0xCF, 0xFD, 0x10, 0xC3, 0x47, 0xFE, 0x91, 0x0A, 0xB3}}
////};
//
//std::vector<Signature> signatures = {
//	{".1", {0xEA, 0x33, 0x17, 0x4E}},
//	{".2", {0x58, 0x6D, 0x57, 0xC9}},
//	{".3", {0x31, 0xF8, 0xFD, 0xFE}},
//	{".4", {0x8B, 0xF4, 0x63, 0x72}},
//	{".5", {0x4A, 0xEB, 0x64, 0xE5}},
//	{".6", {0x48, 0xB0, 0xA5, 0x59}},
//	{".7", {0x48, 0xB9, 0x66, 0xC0}},
//	{".8", {0x40, 0x0F, 0xAE, 0xEB}},
//	{".9", {0x39, 0xD0, 0x0B, 0x08}},
//	{".10", {0x64, 0xC6, 0x47, 0x2A}},
//	{".11", {0xA3, 0xD9, 0xD8, 0xAB}},
//	{".12", {0x0C, 0x23, 0xB4, 0xD0}},
//	{".13", {0x1A, 0x16, 0x81, 0x9E}},
//	{".14", {0x90, 0x4B, 0x88, 0xD8}},
//	{".15", {0xCD, 0x18, 0xFD, 0xC6}},
//	{".16", {0x68, 0x04, 0x0B, 0xFE}},
//	{".17", {0x8B, 0x79, 0x5C, 0xC3}},
//	{".18", {0xA3, 0x03, 0x82, 0xC8}},
//	{".19", {0x5D, 0x1E, 0xCB, 0x23}},
//	{".20", {0x40, 0xD6, 0xB9, 0xF1}},
//	{".21", {0x6D, 0x11, 0xDE, 0x93}},
//	{".22", {0x14, 0x3E, 0xFF, 0xFE}},
//	{".23", {0x33, 0xD7, 0x58, 0x33}},
//	{".24", {0xCD, 0x5D, 0xA0, 0x09}},
//	{".25", {0xF5, 0xCA, 0x21, 0x9F}},
//	{".26", {0x99, 0x28, 0x9D, 0xB7}},
//	{".27", {0xBA, 0xB3, 0xAE, 0x1E}},
//	{".28", {0xB8, 0xB9, 0xE2, 0x11}},
//	{".29", {0x13, 0xD0, 0xAB, 0x6B}},
//	{".30", {0xCF, 0x65, 0xF2, 0xBB}},
//	{".31", {0x81, 0x22, 0x47, 0x5C}},
//	{".32", {0x73, 0xD6, 0x76, 0x39}},
//};
//
//
//std::mutex resultMutex;
//json jsonResult;
//
//std::string toHexString(uint64_t value) {
//	std::stringstream stream;
//	stream << "0x" << std::setfill('0') << std::setw(8) << std::hex << value;
//	return stream.str();
//}
//
//void searchChunk(const uint8_t* data, size_t size, size_t offset, const std::vector<Signature>& sigs) {
//	for (const auto& sig : sigs) {
//		for (size_t i = 0; i <= size - sig.bytes.size(); ++i) {
//			if (memcmp(data + i, sig.bytes.data(), sig.bytes.size()) == 0) {
//				std::lock_guard<std::mutex> lock(resultMutex);
//				jsonResult[sig.name].push_back(toHexString(offset + i));
//			}
//		}
//	}
//}
//
//int main() {
//	//fs::path filePath = "D:\\letleon\\Source\\Cpp\\UnPdeC\\x64\\Debug\\finalcombat.pde";
//	fs::path filePath = "D:\\letleon\\Source\\Cpp\\UnPdeC\\x64\\Debug\\AvatarStar_zh_cn.pde";
//
//
//	// 初始化 jsonResult
//	for (const auto& sig : signatures) {
//		jsonResult[sig.name] = json::array();
//	}
//
//	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE) {
//		std::cerr << "Failed to open file:" << filePath.string() << std::endl;
//		return 1;
//	}
//
//	LARGE_INTEGER fileSize;
//	GetFileSizeEx(hFile, &fileSize);
//
//	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
//	if (hMapping == NULL) {
//		std::cerr << "Failed to create file mapping" << std::endl;
//		CloseHandle(hFile);
//		return 1;
//	}
//
//	LPVOID lpFileBase = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
//	if (lpFileBase == NULL) {
//		std::cerr << "Failed to map view of file" << std::endl;
//		CloseHandle(hMapping);
//		CloseHandle(hFile);
//		return 1;
//	}
//
//	const size_t chunkSize = 1024 * 1024; // 1MB chunks
//	std::vector<std::thread> threads;
//	unsigned int threadCount = std::thread::hardware_concurrency();
//
//	for (size_t i = 0; i < threadCount; ++i) {
//		threads.emplace_back([&, i]() {
//			for (size_t offset = i * chunkSize; offset < fileSize.QuadPart; offset += threadCount * chunkSize) {
//				//size_t size = std::min(chunkSize, static_cast<size_t>(fileSize.QuadPart - offset));
//				size_t size = std::min<size_t>(chunkSize, static_cast<size_t>(fileSize.QuadPart - offset));
//				searchChunk(static_cast<const uint8_t*>(lpFileBase) + offset, size, offset, signatures);
//			}
//			});
//	}
//
//	for (auto& t : threads) {
//		t.join();
//	}
//
//	UnmapViewOfFile(lpFileBase);
//	CloseHandle(hMapping);
//	CloseHandle(hFile);
//
//	// 将JSON写入文件
//	std::ofstream outputFile("search_results1.json");
//	if (outputFile.is_open()) {
//		outputFile << jsonResult.dump(4); // 4 spaces for indentation
//		outputFile.close();
//		std::cout << "Results saved to search_results.json" << std::endl;
//	} else {
//		std::cerr << "Unable to open output file" << std::endl;
//	}
//
//	int totalFound = 0;
//	for (const auto& [key, value] : jsonResult.items()) {
//		totalFound += value.size();
//	}
//	std::cout << "Total signatures found: " << totalFound << std::endl;
//
//	return 0;
//}