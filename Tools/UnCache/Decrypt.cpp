#include "Decrypt.h"

namespace UnCache {
	// 位移表
	uint8_t Decrypt::ByteLimit[16] = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };

	// 获取4字节数据
	std::uint32_t Decrypt::Get4Byte(const std::vector<std::uint8_t>& data, std::uint64_t offset) {
		if (offset + 4 > data.size()) {
			throw std::out_of_range("PdeOffset out of range");
		}
		std::uint32_t value;
		std::memcpy(&value, data.data() + offset, sizeof(value));
		return value;
	}

	std::vector<uint8_t> Decrypt::PreDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName) {
		// 判断哪些文件不需要二次解密
		// 解码后大小标识
		uint8_t SizeFlag = DeTempFileByte[0x18];

		// 如果不等于 0 则需要二次解密
		if ((SizeFlag & 1) != 0) {
			// 待解密的数据
			std::vector<uint8_t> EncryptedData;
			// 解密后的数据大小
			uint64_t DecryptedSize;
			//std::cout << "    File: " << FileName << "\n";

			// 根据标识设置偏移值
			switch (SizeFlag) {
				case 0x6F: // 常规文件? 6F 标识
					// 获取解密后数据长度
					DecryptedSize = Get4Byte(DeTempFileByte, 0x1D);

					// 初始化 待解密的数据 大小
					EncryptedData.resize(DeTempFileByte.size() - 0x21);

					// 从 0x21 开始到 结束 复制到 待解密的数据
					std::copy(DeTempFileByte.begin() + 0x21, DeTempFileByte.end(), EncryptedData.begin());
					break;
				case 0x6D: // 非常规文件? 6D 标识
					// 获取解密后数据长度
					DecryptedSize = DeTempFileByte[0x1A];
					// 初始化EncryptedData大小
					EncryptedData.resize(DeTempFileByte.size() - 0x1B);

					// 从 0x1A 开始到 结束 复制到 EncryptedData
					std::copy(DeTempFileByte.begin() + 0x1B, DeTempFileByte.end(), EncryptedData.begin());
					break;
				default:
					// 未知文件类型
					throw std::runtime_error(" ！解码后大小标识");
			}

			// 解码后数据
			std::vector<uint8_t> DecryptedData;
			// 执行解密
			DecryptedData = FinalDecrypt2(EncryptedData, DecryptedSize, FileName);
			// 删除前 8 个字节,因为8个字节之后才是原始数据
			// 为了能还原回去，所以不删除任何字节
			//DecryptedData.erase(DecryptedData.begin(), DecryptedData.begin() + 8);
			// 返回处理后的字节数组
			return DecryptedData;
		} else { // 不需要二次解密
			//std::vector<uint8_t> result = DeTempFileByte;
			if (DeTempFileByte.size() <= 0x29) {
				std::cout << "初次解密后的字节数组长度不足，无法删除前0x29个字节/n";
				return DeTempFileByte;
			} else {
				// 删除前0x29个字节
				// 为了能还原回去，所以不删除任何字节
				//std::vector<uint8_t> result(std::next(DeTempFileByte.begin(), 0x29), DeTempFileByte.end());
				return DeTempFileByte;
			}

			std::cout << " ！" << FileName << " 无需二次解密" << "\n";
		}
	}

	std::vector<uint8_t> Decrypt::FinalDecrypt2(const std::vector<uint8_t>& encryptedData, uint64_t decryptedSize, const std::string& FileName) {
		try {
			std::vector<uint8_t> decryptedData(decryptedSize);
			uint32_t EAX = 1;
			uint32_t EBX = 1;
			uint32_t ECX = 0;
			uint32_t EDX = 0;
			uint32_t EBP = 0;
			uint32_t ESI = 0;
			uint32_t EDI = static_cast<uint32_t>(decryptedSize);
			uint32_t ESP58 = 0;

			while (true) {
				if (EAX == 1) {
					if (ESI + 4 > encryptedData.size()) break; // Check bounds
					EAX = Get4Byte(encryptedData, ESI);
					ESI += 4;
				}

				if (ESI + 4 > encryptedData.size()) break; // Check bounds
				ECX = Get4Byte(encryptedData, ESI);

				if ((EAX & 1) == 0) {
					if (EBP >= EDI - 10) break; // Check if there's room for more data
					memcpy(decryptedData.data() + EBP, &ECX, sizeof(ECX));
					uint32_t shiftAmount = ByteLimit[EAX & 0xF];
					EBP += shiftAmount;
					ESI += shiftAmount;
					EAX >>= shiftAmount;
				} else {
					EAX >>= 1;
					ESP58 = EAX;
					uint32_t offset = 0;
					uint32_t length = 0;

					if ((ECX & 3) != 0) {
						if ((ECX & 2) != 0) {
							if ((ECX & 1) != 0) {
								if ((ECX & 0x7F) == 3) {
									offset = ECX >> 15;
									length = ((ECX >> 7) & 0xFF) + 3;
									ESI += 4;
								} else {
									offset = (ECX >> 7) & 0x1FFFF;
									length = ((ECX >> 2) & 0x1F) + 2;
									ESI += 3;
								}
							} else {
								offset = (ECX >> 6) & 0x3FF;
								length = ((ECX >> 2) & 0xF) + 3;
								ESI += 2;
							}
						} else {
							offset = (ECX >> 2) & 0x3FFF;
							length = 3;
							ESI += 2;
						}
					} else {
						offset = (ECX >> 2) & 0x3F;
						length = 3;
						ESI += 1;
					}

					uint32_t srcIndex = EBP - offset;
					if (srcIndex >= decryptedData.size()) break; // Check bounds

					for (uint32_t i = 0; i < length; ++i) {
						if (EBP + i >= decryptedData.size() || srcIndex + i >= decryptedData.size()) break; // Prevent overflow
						decryptedData[EBP + i] = decryptedData[srcIndex + i];
					}

					EBP += length;
					EAX = ESP58;
				}
			}

			while (EBP < EDI) {
				if (EAX == 1) {
					if (ESI + 4 > encryptedData.size()) break;
					EAX = 0x80000000;
					ESI += 4;
				}
				if (ESI >= encryptedData.size()) break;
				decryptedData[EBP] = encryptedData[ESI];
				++EBP;
				++ESI;
				EAX >>= 1;
			}

			return decryptedData;

		} catch (const std::exception& e) {
			throw std::runtime_error(" ！Decrypt::FinalDecrypt2() 函数执行出错：" + std::string(e.what()));
		}
	}
}