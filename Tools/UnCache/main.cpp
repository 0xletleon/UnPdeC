#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <iostream>

// 移位表
static constexpr uint8_t SHIFT_TABLE[16] = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

// 寄存器
struct RegisterState {
	uint32_t eax;    // 控制位寄存器
	uint32_t ecx;    // 当前数据
	uint32_t ebp;    // 写入位置
	uint32_t esi;    // 读取位置
	uint32_t edi;    // 目标大小
};

// 获取4字节数据
static uint32_t Get4Byte(const std::vector<uint8_t>& data, uint64_t offset) {
	if (offset + 4 > data.size()) {
		throw std::out_of_range("Offset out of range");
	}
	uint32_t value;
	std::memcpy(&value, data.data() + offset, sizeof(value));
	return value;
}

// 解压缩
static std::vector<uint8_t> Decompress(const std::vector<uint8_t>& compressedData, uint32_t decompressedSize, const std::string& fileName) {
	try {
		std::vector<uint8_t> outputBuffer(decompressedSize);
		RegisterState regs = { 1, 0, 0, 0, decompressedSize };

		const uint8_t* srcPtr = compressedData.data();
		uint8_t* dstPtr = outputBuffer.data();

		// 模拟原32位程序的解压逻辑
		while (true) {
			// 读取新的控制位（模拟原始的EAX寄存器操作）
			if (regs.eax == 1) {
				if (regs.esi + sizeof(uint32_t) > compressedData.size()) break;
				std::memcpy(&regs.eax, &srcPtr[regs.esi], sizeof(uint32_t));
				regs.esi += sizeof(uint32_t);
			}

			// 读取当前数据块（模拟原始的ECX寄存器操作）
			if (regs.esi + sizeof(uint32_t) > compressedData.size()) break;
			std::memcpy(&regs.ecx, &srcPtr[regs.esi], sizeof(uint32_t));

			// 处理直接复制的情况
			if ((regs.eax & 1) == 0) {
				if (regs.ebp >= regs.edi - 10) break;

				uint32_t shiftAmount = SHIFT_TABLE[regs.eax & 0xF];

				// 逐个字节复制
				for (uint32_t i = 0; i < shiftAmount; ++i) {
					if (regs.ebp + i >= decompressedSize) break;
					dstPtr[regs.ebp + i] = srcPtr[regs.esi + i];
				}

				regs.ebp += shiftAmount;
				regs.esi += shiftAmount;
				regs.eax >>= shiftAmount;
			}
			// 处理重复数据的情况
			else {
				uint32_t savedBits = regs.eax >> 1;
				uint32_t offset = 0;
				uint32_t length = 0;

				// 保持原始的位操作逻辑
				if ((regs.ecx & 3) != 0) {
					if ((regs.ecx & 2) != 0) {
						if ((regs.ecx & 1) != 0) {
							if ((regs.ecx & 0x7F) == 3) {
								offset = regs.ecx >> 15;
								length = ((regs.ecx >> 7) & 0xFF) + 3;
								regs.esi += 4;
							} else {
								offset = (regs.ecx >> 7) & 0x1FFFF;
								length = ((regs.ecx >> 2) & 0x1F) + 2;
								regs.esi += 3;
							}
						} else {
							offset = (regs.ecx >> 6) & 0x3FF;
							length = ((regs.ecx >> 2) & 0xF) + 3;
							regs.esi += 2;
						}
					} else {
						offset = (regs.ecx >> 2) & 0x3FFF;
						length = 3;
						regs.esi += 2;
					}
				} else {
					offset = (regs.ecx >> 2) & 0x3F;
					length = 3;
					regs.esi += 1;
				}

				//// 安全性检查
				//if (regs.ebp < offset || regs.ebp + length > decompressedSize) break;
				// 检查offset是否有效
				if (offset > regs.ebp || regs.ebp + length > decompressedSize) {
					break;
				}

				// 严格按照原始方式复制数据
				for (uint32_t i = 0; i < length; ++i) {
					dstPtr[regs.ebp + i] = dstPtr[regs.ebp - offset + i];
				}

				regs.ebp += length;
				regs.eax = savedBits;
			}
		}

		// 处理剩余数据
		while (regs.ebp < regs.edi) {
			if (regs.eax == 1) {
				if (regs.esi + sizeof(uint32_t) > compressedData.size()) break;
				regs.eax = 0x80000000;
				regs.esi += sizeof(uint32_t);
			}
			if (regs.esi >= compressedData.size()) break;
			dstPtr[regs.ebp++] = srcPtr[regs.esi++];
			regs.eax >>= 1;
		}

		return outputBuffer;
	} catch (const std::exception& e) {
		throw std::runtime_error(
			std::format("Decompression failed for {}: {}",
				fileName, e.what())
		);
	}
}

// 预处理
static std::vector<uint8_t> PreProcess(const std::vector<uint8_t>& fileData, const std::string& fileName) {
	if (fileData.size() < 0x1B) {
		throw std::runtime_error("File data too short");
	}

	// 获取Flag
	uint8_t sizeFlag = fileData[0x18];
	// 待解压数据
	std::vector<uint8_t> encryptedData;
	// 解压后大小
	uint32_t decryptedSize;
	std::vector<uint8_t> decryptedData;

	// 不同的Flag,不同的标识地址
	if (sizeFlag == 0x6F) {
		if (fileData.size() < 0x21) {
			throw std::runtime_error("0x6F flag file data too short");
		}
		// 解压后大小
		decryptedSize = Get4Byte(fileData, 0x1D);
		encryptedData.assign(fileData.begin() + 0x21, fileData.end());
	} else if (sizeFlag == 0x6D) {
		// 解压后大小
		decryptedSize = fileData[0x1A];
		encryptedData.assign(fileData.begin() + 0x1B, fileData.end());
	} else if (sizeFlag == 0x6E) {
		// 删除 fileData 前0x29字节，并返回
		return std::vector<uint8_t>(fileData.begin() + 0x29, fileData.end());
	} else {
		std::cout << "Unknown size flag: " << std::to_string(sizeFlag) << "\n";
		throw std::runtime_error("Unknown size flag: " + std::to_string(sizeFlag));
	}

	// 按照汇编 004CFF42 处的逻辑实现
	// 大于0xF4240
	if (decryptedSize - 0x1 > 0xF4240) {
		std::cout << "大于0xF4240\n";

		// 检查最低位是否为 0
		if ((sizeFlag & 0x01) == 0) {
			// 最低位为 0，执行跳转逻辑，不执行解压
			std::cout << "最低位为 0，执行跳转逻辑，不执行解压\n";
			// 在这里添加跳转目标的代码
		} else {
			// 最低位为 1，继续执行,继续解压
			std::cout << "最低位为 1，继续执行,继续解压\n";
			// 解压缩函数
			decryptedData = Decompress(encryptedData, decryptedSize, fileName);
		}
	} else {// 小于0xF4240
		std::cout << "小于0xF4240\n";

		// 检查最低位是否为 0
		if ((sizeFlag & 0x01) == 0) {
			// 最低位为 0，执行跳转逻辑，不执行解压
			std::cout << "最低位为 0，执行跳转逻辑，不执行解压\n";
			// 在这里添加跳转目标的代码
		} else {
			// 最低位为 1，继续执行,继续解压
			std::cout << "最低位为 1，继续执行,继续解压\n";
			// 解压缩函数
			decryptedData = Decompress(encryptedData, decryptedSize, fileName);
		}

	}

	//// 解压缩函数
	//auto decryptedData = Decompress(encryptedData, decryptedSize, fileName);

	// 删除前八字节
	// 前8个字节包含了文件类型和全部数据的字节大小(不包含这个8个字节)
	decryptedData.erase(decryptedData.begin(), decryptedData.begin() + 8);
	// 返回解压后的文件
	return decryptedData;
}

// 主函数
int main(int argc, char* argv[]) {
	// 调试时使用的文件路径
	std::string debugFilePath = "D:\\letleon\\Source\\Cpp\\UnPdeC\\Tools\\UnCache\\bin\\Debug\\Win32\\battlefield_map_lv33.dds.cache";

	// 如果命令行参数不足，使用调试文件路径
	if (argc != 2) {
		std::cout << "Usage: UnCache <input_file.cache>\nOr set the .cache file to open as UnCache.exe\n";
		std::cout << "Using debug file: " << debugFilePath << "\n";
		argv[1] = const_cast<char*>(debugFilePath.c_str());
	}

	try {
		std::filesystem::path input_file = argv[1];
		if (!std::filesystem::exists(input_file)) {
			throw std::runtime_error("Input file not found");
		}

		auto filename = input_file.filename().string();
		auto directory = input_file.parent_path();
		std::cout << "Processing file: " << filename << "\nOutput directory : " << directory << "\n";

		// 读取文件
		std::ifstream file(input_file, std::ios::binary);
		std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		std::cout << "Raw file size: " << fileData.size() << " bytes\n";

		// 解压文件
		auto decryptedData = PreProcess(fileData, filename);
		std::cout << "Decrypted data size: " << decryptedData.size() << " bytes\n";

		// 写入解压后的文件
		auto output_file = directory / filename.substr(0, filename.length() - 6); // 移除.cache扩展名
		std::cout << "Writing to: " << output_file << "\n";

		std::ofstream outFile(output_file, std::ios::binary);
		if (!outFile) {
			throw std::runtime_error("Failed to create output file");
		}
		outFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
		outFile.close();

		if (std::filesystem::exists(output_file)) {
			std::cout << "Successfully created file: " << output_file << "\n";
			std::cout << "File size: " << std::filesystem::file_size(output_file) << " bytes\n";
		} else {
			throw std::runtime_error("Failed to verify output file");
		}

	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << "\n";
		std::cin;
		return 1;
	}

	return 0;
}
