//#include "FinalUnpack.h"
//
//namespace UnPdeC {
//
//	static std::vector<uint8_t> FinalUnpack::FinalDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName) {
//		std::cout << "���ڶ��ν��ܵ��ļ���: " << FileName << std::endl;
//		uint8_t REG_AL = DeTempFileByte[0x18];
//		std::cout << "REG_AL: 0x" << std::hex << static_cast<int>(REG_AL) << std::endl;
//
//		if ((REG_AL & 1) != 0) {
//			return FinalDecrypt2(DeTempFileByte);
//		} else {
//			if (DeTempFileByte.size() <= 0x29) {
//				throw std::runtime_error("���ν��ܺ���ֽ����鳤�Ȳ��㣬�޷�ɾ��ǰ0x29���ֽ�");
//			}
//			std::vector<uint8_t> result(DeTempFileByte.begin() + 0x29, DeTempFileByte.end());
//			std::cout << FileName << " ������ν���" << std::endl;
//			return result;
//		}
//	}
//
//	static std::vector<uint8_t> FinalUnpack::FinalDecrypt2(const std::vector<uint8_t>& DeTempFileByte) {
//
//	}
//}