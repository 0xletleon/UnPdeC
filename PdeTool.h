// PdeTool.h
#ifndef PDETOOL_H
#define PDETOOL_H

#include "Common.h"
#include "GV.h"
#include "PdeKey.h"

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

		//// ��ȡ�ļ����ļ���ƫ����Ϣ
		//static std::vector<HexOffsetInfo> GetOffsetInfo(const std::vector<unsigned char>& data, uint32_t BlockOffset);

		//// ��PDE�ļ��л�ȡָ��������
		//static GetOffsetStr GetByteOfPde(uint32_t Start, uint32_t Size);

		//// �����ļ������ݿ�
		//static std::vector<unsigned char> DeFileOrBlock(const std::vector<unsigned char>& OffsetArr);

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