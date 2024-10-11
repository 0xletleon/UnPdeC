#pragma once

#include "Common.h"
#include "GV.h"
#include "Tools.h"
#include "Decrypt.h"

namespace UnPdeC {
	/// <summary>
	/// PDE������
	/// </summary>
	class Mata {
	public:
		/// <summary>
		/// ��ȡԪ��Ϣ
		/// </summary>
		/// <param name="MataBlock">������</param>
		/// <param name="XorFileSize">.xor�ļ���С</param>
		/// <returns>Ԫ��Ϣ</returns>
		static BlockInfo ExtractMataInfo(const uint8_t* MataBlock, uint64_t XorFileSize);

		/// <summary>
		/// ��ȡ.xor������Ԫ���ݿ�
		/// </summary>
		/// <param name="FilePath">.xor�ļ�·��</param>
		/// <returns>�����ļ�/�е�Ԫ����json</returns>
		static json ReadMatakForXorFile(const std::filesystem::path& FilePath);

		/// <summary>
		/// ���벢�����ļ�/��
		/// </summary>
		/// <param name="MataJson">�����ļ���Ԫ��Ϣ</param>
		static void DecodeAndSaveFile(const MataSaveInfo& MataJson);

		/// <summary>
		/// ��ȡԪ��Ϣ������
		/// </summary>
		/// <param name="XorFileMatas">Xor�ļ�Ԫ��Ϣ</param>
		static void ExtractMateAndDecode(const json& XorFileMatas);
	};
}