// GV.cpp
#include "GV.h"

namespace UnPdeC {
	// ȫ�ֱ����Ƿ���Ҫ����
	bool GV::NeedDebugPde = false;
	// �Ƿ���Ҫ���ɵ�����Ϣ
	bool GV::NeedOffsetLog = false;
	// �Ƿ���Ҫ���ɽ��ܺ��PDE����
	bool GV::NeedFindDirs = false;
	// ��ǰ���ڴ����PDE
	TNowPde GV::NowPde = { "", 0L };
	// ������Կ
	vector<unsigned char> GV::PdeKey = vector<unsigned char>(1000, 0);
}