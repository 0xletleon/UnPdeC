// FinalUnpack.cpp
#include "FinalUnpack.h"

namespace UnPdeC {
	std::vector<uint8_t> FinalUnpack::PreDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName) {
		std::cout << "正在二次解密的文件名: " << FileName << std::endl;

		// 判断哪些文件不需要二次解密
		// 解码后大小标识
		uint8_t SizeFlag = DeTempFileByte[0x18];
		// std::cout << " ！SizeFlag:"<< std::hex<< static_cast<int>(SizeFlag)<< std::endl;

		// 如果不等于 0 则需要二次解密
		if ((SizeFlag & 1) != 0) {
			// 待解密的数据
			std::vector<uint8_t> EncryptedData;
			// 解密后的数据大小
			uint32_t DecryptedSize;

			// 根据标识设置偏移值
			switch (SizeFlag) {
			case 0x6F: // 常规文件? 6F 标识
				// 获取解密后数据长度
				DecryptedSize = UFunc::Get4Byte(DeTempFileByte, 0x1D);

				// 初始化 待解密的数据 大小
				EncryptedData.resize(DeTempFileByte.size() - 0x21);
				// 从 0x21 开始到 结束 复制到 待解密的数据
				std::copy(DeTempFileByte.begin() + 0x21, DeTempFileByte.end(), EncryptedData.begin());
				break;
			case 0x6D: // 非常规文件? 6D 标识
				// 获取解密后数据长度
				 //todo: 类型
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

			// 执行二解
			std::vector<uint8_t> DecryptedData;
			DecryptedData = FinalDecrypt2(EncryptedData, DecryptedSize);

			// 删除前 8 个字节
			DecryptedData.erase(DecryptedData.begin(), DecryptedData.begin() + std::min(static_cast<size_t>(8), DecryptedData.size()));

			// 返回处理后的字节数组
			return DecryptedData;
		} else { // 不需要二次解密
			if (DeTempFileByte.size() <= 0x29) {
				throw std::runtime_error("初次解密后的字节数组长度不足，无法删除前0x29个字节");
			}
			// 删除前0x29个字节
			std::vector<uint8_t> result(DeTempFileByte.begin() + 0x29, DeTempFileByte.end());
			std::cout << FileName << " 无需二次解密" << std::endl;

			// 返回处理后的字节数组
			return result;
		}
	}

	std::vector<uint8_t> FinalUnpack::FinalDecrypt2(const std::vector<uint8_t>& encryptedData, uint32_t decryptedSize) {
		// 位移表
		const unsigned char ByteLimt[16] = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
		// 解码后数据
		std::vector<uint8_t> decryptedData(decryptedSize);

		// 寄存器
		// 待解开始地址
		uint32_t EAX = 1;
		uint32_t EBX = 1;
		uint32_t ECX = 0;
		// 退出条件
		uint32_t EDX;
		// 下一个数据写入的地址
		uint32_t EBP = 0;
		// 当前待解地址
		uint32_t ESI = 0;
		// 解密后数据结束地址
		uint32_t EDI = decryptedSize - 1;
		// 写入地址
		uint32_t ESP10 = EDI;
		// 解码后数据大小
		uint32_t ESP58 = decryptedSize;

		while (true) {
			while (true) {
				if (EAX == EBX) {
					EAX = UFunc::Get4Byte(encryptedData, ESI);
					ESI += 4;
				}
				ECX = UFunc::Get4Byte(encryptedData, ESI);

				if (((EBX & 0xFF) & (EAX & 0xFF)) == 0) break;

				EAX >>= 1;
				ESP58 = EAX;

				if (((ECX & 0xFF) & 3) != 0) {
					if (((ECX & 0xFF) & 2) != 0) {
						EDX = ECX;
						if (((EBX & 0xFF) & (ECX & 0xFF)) != 0) {
							EDX &= 0X7F;
							if ((EDX & 0xFF) == 3) {
								EDX = ECX;// EDX & 0x7F 之后 判断，之后来到这里在将ECX赋值给EDX
								ECX >>= 7;
								ECX &= 0xFF;
								EDX >>= 0xF;
								ECX += 0x3;
								ESI += 0x4;
								// -> 99 ok
							} else {
								EDX = ECX;// ！
								ECX >>= 2;
								EDX >>= 7;
								ECX &= 0x1F;
								EDX &= 0x1FFFF;
								ECX += 2;
								ESI += 3;
								// -> 99
							}
						} else {
							ECX >>= 2;
							EDX >>= 6;
							ECX &= 0xF;
							EDX &= 0x3FF;
							ECX += 0x3;
							ESI += 0x2;
							// -> 99 ok
						}
					} else {
						ECX >>= 2;
						ECX &= 0x3FFF;
						EDX = ECX;
						ECX = 3;
						ESI += 2;
						// -> 99
					}
				} else {
					ECX >>= 2;
					ECX &= 0x3F;
					EDX = ECX;
					ECX = 3;
					ESI += EBX;
					// -> 99 ok
				}

				// 99
			/*	cout << "!EBP:" << EBP << endl;
				if (EBP == 0x7a4) {
					cout << "找到0x79f" << endl;
				}*/
				EDI = EBP;
				EDI -= EDX;
				EBX = 0;
				EDX = EBP;

				EDI -= EBP;
				do {
					uint32_t EDIX = EDI + EDX;
					EAX = UFunc::Get4Byte(decryptedData, EDIX);
					//cout << "EDI:" << std::hex << EDI << " EDX:" << std::hex << EDX << std::hex << " EDIX:" << EDIX << endl;
					//cout << "-> EAX:" << std::hex << EAX << endl;
					// 根据 EDX 下标 将 EAX 写入 decryptedData
					memcpy(decryptedData.data() + EDX, &EAX, sizeof(EAX));
					EBX += 3;
					EDX += 3;
				} while (EBX < ECX);
				EAX = ESP58;
				EDI = ESP10;
				EBP += ECX;
				EBX = 1;
			}

			// 退出条件
			EDX = EDI - 0xA;
			if (EBP >= EDX)break;

			// 根据 EBP 下标 将 ECX 写入 decryptedData
			memcpy(decryptedData.data() + EBP, &ECX, sizeof(ECX));
			ECX = EAX;
			ECX &= 0xF;
			ECX = ByteLimt[ECX];
			EBP += ECX;// 似乎是错的！
			ESI += ECX;
			//EAX >>= (ECX & 0xFF);
			EAX >>= ECX;
		}

		cout << "到达0x80000000" << endl;
		if (EBP <= EDI) {
			do {
				if (EAX == EBX) {
					ESI += 4;
					EAX = 0x80000000;
				}
				uint8_t DL = encryptedData[ESI];
				// 根据 EBP下标 将 DL 写入 decryptedData
				memcpy(decryptedData.data() + EBP, &DL, sizeof(DL));
				EBP += EBX;
				ESI += EBX;
				EAX >>= 1;
			} while (EBP <= EDI);
		}

		cout << "完成二解" << endl;

		return decryptedData;
	}
}