//#include "FinalUnpack.h"
//
//namespace UnPdeC {
//
//	static std::vector<uint8_t> FinalUnpack::FinalDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName) {
//		std::cout << "正在二次解密的文件名: " << FileName << std::endl;
//		uint8_t REG_AL = DeTempFileByte[0x18];
//		std::cout << "REG_AL: 0x" << std::hex << static_cast<int>(REG_AL) << std::endl;
//
//		if ((REG_AL & 1) != 0) {
//			return FinalDecrypt2(DeTempFileByte);
//		} else {
//			if (DeTempFileByte.size() <= 0x29) {
//				throw std::runtime_error("初次解密后的字节数组长度不足，无法删除前0x29个字节");
//			}
//			std::vector<uint8_t> result(DeTempFileByte.begin() + 0x29, DeTempFileByte.end());
//			std::cout << FileName << " 无需二次解密" << std::endl;
//			return result;
//		}
//	}
//
//	static std::vector<uint8_t> FinalUnpack::FinalDecrypt2(const std::vector<uint8_t>& DeTempFileByte) {
//
//	}
//}