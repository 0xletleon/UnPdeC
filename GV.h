// GV.h
#ifndef GV_H
#define GV_H

#include "Common.h"
#include "FindPde.h"

namespace UnPdeC {
	/// <summary>
	/// ȫ�ֱ�����
	/// </summary>
	class GV {
	public:
		// �Ƿ���Ҫ�����õ�
		static bool NeedDebugPde;
		// �Ƿ���Ҫ��¼ƫ����־�ļ�
		static bool NeedOffsetLog;
		// �Ƿ���Ҫ����δ�����Ŀ¼
		static bool NeedFindDirs;
		// ��ǰ���ڴ����PDE�ļ���
		static TNowPde NowPde;
		// ������Կ
		static vector<unsigned char> PdeKey;
	};
}
#endif // GV_H