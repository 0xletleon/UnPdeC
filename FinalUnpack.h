#ifndef FINAL_UNPACK_H
#define FINAL_UNPACK_H

#include "Common.h"
#include "UFunc.h"

namespace UnPdeC {

	class FinalUnpack {
	private:
		// ���ν��ܷ���
		static std::vector<uint8_t> FinalDecrypt2(const std::vector<uint8_t>& DeTempFileByte, uint32_t);
	public:
		/// <summary>
		/// ���ν���Ԥ����
		/// </summary>
		/// <param name="DeTempFileByte">�����ܵ�ԭʼ����</param>
		/// <param name="FileName">�ļ���</param>
		/// <returns>���ܺ������</returns>
		static std::vector<uint8_t> PreDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName);
	};

} // namespace UnPdeC

#endif // FINAL_UNPACK_H