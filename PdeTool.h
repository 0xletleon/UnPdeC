// PdeTool.h
#ifndef PDETOOL_H
#define PDETOOL_H

#include "Common.h"
#include "GV.h"
#include "PdeKey.h"
#include "NameValidator.h"

// ����DataType��PdeNames�������Ѿ����ʵ��ĵط�����
namespace UnPdeC {

	/// <summary>
	/// PDE������
	/// </summary>
	class PdeTool {
		//private:
			//static uint32_t PDESIZE;
			//static std::vector<unsigned char> PDEKEY;
			//static PdeNames THISPDENAME;

	public:
		// �ṩ�Ծ�̬��Ա�ķ����������������Ҫ�Ļ�
		//static PdeNames ThisPdeName();

		// ��ʼ��PDE������
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

		// ����������Ա�����ͱ���...

		// ˽�й��캯�����Է�ֹʵ����
		//PdeTool() = delete;
	};

	// ������������̬��Ա�����Ķ��壨ʵ�֣�����.cpp�ļ���
   /* extern uint32_t PdeTool::PDESIZE;
	extern std::vector<unsigned char> PdeTool::PDEKEY;
	extern PdeNames PdeTool::THISPDENAME;*/

} // namespace Unpde

#endif // PDETOOL_H