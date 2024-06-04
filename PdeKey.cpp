// PdeKey.cpp
#include "PdeKey.h"

namespace UnPdeC {
	void PdeKey::Get() {
		// 初始化为0
		vector<unsigned char> KeyByte(0x1000, 0);

		// 逻辑从 汇编 0x00A608E0 处获得
		uint32_t EAX = 0x42574954;
		for (size_t i = 0; i < 0x1000; ++i) {
			EAX *= 0x7FCF;
			uint32_t ECX = EAX >> 24; // 获取ECX的高8位
			uint32_t EDX = (EAX >> 16) & 0xFF; // 获取EDX的次高8位
			uint32_t CL = ECX ^ EDX; // CL = ECX ^ EDX
			CL ^= (EAX >> 8) & 0xFF; // CL = CL ^ EDX的高8位
			CL ^= EAX & 0xFF; // CL = CL ^ EAX的低8位
			KeyByte[i] = static_cast<unsigned char>(CL);
		}

		cout << " √PDEKEY计算完成" << endl;
		// 给全局变量
		GV::PdeKey = KeyByte;
	}
}