// PdeKey.cpp
#include "PdeKey.h"

namespace UnpdeC {
	using std::vector;
	using std::cout;
	using std::endl;

	vector<unsigned char> PdeKey::PDEKEY() {
		// ��ʼ��Ϊ0
		vector<unsigned char> KeyByte(0x1000, 0);

		// �߼��� ��� 0x00A608E0 �����
		uint32_t EAX = 0x42574954;
		for (size_t i = 0; i < 0x1000; ++i) {
			EAX *= 0x7FCF;
			uint32_t ECX = EAX >> 24; // ��ȡECX�ĸ�8λ
			uint32_t EDX = (EAX >> 16) & 0xFF; // ��ȡEDX�Ĵθ�8λ
			uint32_t CL = ECX ^ EDX; // CL = ECX ^ EDX
			CL ^= (EAX >> 8) & 0xFF; // CL = CL ^ EDX�ĸ�8λ
			CL ^= EAX & 0xFF; // CL = CL ^ EAX�ĵ�8λ
			KeyByte[i] = static_cast<unsigned char>(CL);
		}

		cout << " ��PDEKEY�������" << endl;
		return KeyByte;
	}

} // namespace UnpdeC