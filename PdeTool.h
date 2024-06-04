// PdeTool.h
#ifndef PDETOOL_H
#define PDETOOL_H

#include "Common.h"
#include "GV.h"
#include "PdeKey.h"
#include "NameValidator.h"

namespace UnPdeC {

	/// <summary>
	/// PDE������
	/// </summary>
	class PdeTool {
	public:
		/// <summary>
		/// ��ʼ��PDE������
		/// </summary>
		static void Init();

		/// <summary>
		/// ��ȡ�ļ����ļ���ƫ����Ϣ
		/// </summary>
		/// <param name="data">���ν��ܺ������</param>
		/// <param name="BlockOffset">���ݿ���PDE�ļ��е�ƫ��ֵ</param>
		/// <returns>�ļ����ļ��е�ƫ����Ϣ���� </returns>
		static std::vector<HexOffsetInfo> GetOffsetInfo(const std::vector<unsigned char>& data, uint32_t BlockOffset);

		/// <summary>
		/// ��PDE�ļ��л�ȡָ��������
		/// </summary>
		/// <param name="Start">����PDE�ļ��е���ʼƫ��</param>
		/// <param name="Size">���С</param>
		/// <returns>������</returns>
		static GetOffsetStr GetByteOfPde(uint32_t Start, uint32_t Size);

		/// <summary>
		/// �����ļ� ��! ���ݿ�
		/// </summary>
		/// <param name="OffsetArr">�ļ������ݿ��ƫ����Ϣ����</param>
		/// <returns>���ܺ���ֽ�����</returns>
		static std::vector<unsigned char> DeFileOrBlock(const std::vector<unsigned char>& OffsetArr);
	};

} // namespace UnPdeC

#endif // PDETOOL_H