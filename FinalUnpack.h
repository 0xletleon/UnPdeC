#ifndef FINAL_UNPACK_H
#define FINAL_UNPACK_H

#include "Common.h"

namespace UnPdeC {

	class FinalUnpack {
	private:
		// ���ν��ܷ���
		static std::vector<uint8_t> FinalDecrypt2(const std::vector<uint8_t>& DeTempFileByte);
		//static std::vector<uint8_t> FinalDecrypt3(const std::vector<uint8_t>& DeTempFileByte);
	public:
		// ���ս����ж�
		static std::vector<uint8_t> FinalDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName);
	};

} // namespace Unpde

#endif // FINAL_UNPACK_H