#include "FinalUnpack.h"

namespace UnPdeC {
	std::vector<uint8_t> FinalUnpack::PreDecrypt(const std::vector<uint8_t>& DeTempFileByte, const std::string& FileName) {
		std::cout << "���ڶ��ν��ܵ��ļ���: " << FileName << std::endl;

		// �ж���Щ�ļ�����Ҫ���ν���
		// ������С��ʶ
		uint8_t SizeFlag = DeTempFileByte[0x18];
		// std::cout << " ��SizeFlag:"<< std::hex<< static_cast<int>(SizeFlag)<< std::endl;

		// ��������� 0 ����Ҫ���ν���
		if ((SizeFlag & 1) != 0) {

			// �����ܵ�����
			std::vector<uint8_t> EncryptedData;
			// ���ܺ�����ݴ�С
			uint32_t DecryptedSize;

			// ���ݱ�ʶ����ƫ��ֵ
			switch (SizeFlag) {
			case 0x6F: // �����ļ�? 6F ��ʶ
				// ��ȡ���ܺ����ݳ���
				DecryptedSize = UFunc::Get4Byte(DeTempFileByte, 0x1D);

				// ��ʼ�� �����ܵ����� ��С
				EncryptedData.resize(DeTempFileByte.size() - 0x21);
				// �� 0x21 ��ʼ�� ���� ���Ƶ� �����ܵ�����
				std::copy(DeTempFileByte.begin() + 0x21, DeTempFileByte.end(), EncryptedData.begin());
				break;
			case 0x6D: // �ǳ����ļ�? 6D ��ʶ
				// ��ȡ���ܺ����ݳ���
				 //todo: ����
				DecryptedSize = DeTempFileByte[0x1A];
				// ��ʼ��EncryptedData��С
				EncryptedData.resize(DeTempFileByte.size() - 0x1B);
				// �� 0x1A ��ʼ�� ���� ���Ƶ� EncryptedData
				std::copy(DeTempFileByte.begin() + 0x1B, DeTempFileByte.end(), EncryptedData.begin());
				break;
			default:
				// δ֪�ļ�����
				throw std::runtime_error(" ��������С��ʶ");
			}

			// ִ�ж���
			return FinalDecrypt2(EncryptedData, DecryptedSize);
		} else { // ����Ҫ���ν���
			if (DeTempFileByte.size() <= 0x29) {
				throw std::runtime_error("���ν��ܺ���ֽ����鳤�Ȳ��㣬�޷�ɾ��ǰ0x29���ֽ�");
			}
			// ɾ��ǰ0x29���ֽ�
			std::vector<uint8_t> result(DeTempFileByte.begin() + 0x29, DeTempFileByte.end());
			std::cout << FileName << " ������ν���" << std::endl;

			// ���ش������ֽ�����
			return result;
		}
	}

	std::vector<uint8_t> FinalUnpack::FinalDecrypt2(const std::vector<uint8_t>& encryptedData, uint32_t decryptedSize) {
		// λ�Ʊ�
		const unsigned char ByteLimt[16] = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
		// ���������
		std::vector<uint8_t> decryptedData(decryptedSize);

		// �Ĵ���
		// ���⿪ʼ��ַ
		uint32_t EAX = 1;
		uint32_t EBX = 1;
		uint32_t ECX = 0;
		// �˳�����
		uint32_t EDX;
		// ��һ������д��ĵ�ַ
		uint32_t EBP = 0;
		// ��ǰ�����ַ
		uint32_t ESI = 0;
		// ���ܺ����ݽ�����ַ
		uint32_t EDI = decryptedSize - 1;
		// д���ַ
		uint32_t ESP10 = EDI;
		// ��������ݴ�С
		uint32_t ESP58 = decryptedSize;

		while (true) {
			while (true) {
				if (EAX == EBX) {
					EAX = UFunc::Get4Byte(encryptedData, ESI);
					ESI += 4;
				}
				ECX = UFunc::Get4Byte(encryptedData, ESI);

				if (((EAX & 0xFF) & (EBX & 0xFF)) == 0) break;

				EAX >>= 1;
				ESP58 = EAX;

				if (((ECX & 0xFF) & 3) != 0) {
					if (((ECX & 0xFF) & 2) != 0) {
						EDX = ECX;
						if (((EBX & 0xFF) & (ECX & 0xFF)) != 0) {
							EDX &= 0X7F;
							EDX = ECX;
							if ((EDX & 0xFF) == 0x3) {
								ECX >>= 7;
								ECX &= 0xFF;
								EDX >>= 0xF;
								ECX += 0x3;
								ESI += 0x4;
								// -> 99
							} else {
								ECX >>= 2;
								EDX >>= 7;
								ECX &= 0x1F;
								EDX &= 0x1FFF;
								ECX += 2;
								ESI += 3;
								// -> 99
							}
						} else {
							ECX >>= 2;
							EDX >>= 6;
							ECX &= 0xF;
							EDX &= 0x3FF;
							ECX += 0x3;
							ESI += 0x2;
							// -> 99
						}
					} else {
						ECX >>= 2;
						ECX &= 0x3FFF;
						EDX = ECX;
						ECX = 3;
						ESI += 2;
						// -> 99
					}
				} else {
					ECX >>= 2;
					ECX &= 0x3F;
					EDX = ECX;
					ECX = 3;
					ESI += EBX;
					// -> 99 
				}

				// 99
				EDI = EBP;
				EDI -= EDX;
				EBX = 0;
				EDX = EBP;
				EDI -= EBP;
				do {
					EAX = UFunc::Get4Byte(decryptedData, static_cast<size_t>(EDI + EDX));
					// ���� EDX �±� �� EAX д�� decryptedData
					memcpy(decryptedData.data() + EDX, &EAX, sizeof(EAX));
					EBX += 3;
					EDX += 3;
				} while (EBX < ECX);
				EAX = ESP58;
				EDI = ESP10;
				EBP += ECX;
				EBX = 1;
			}

			// �˳�����
			EDX = EDI - 0xA;
			if (EBP >= EDX)break;

			// ���� EBP �±� �� ECX д�� decryptedData
			memcpy(decryptedData.data() + EBP, &ECX, sizeof(ECX));
			ECX = EAX;
			ECX &= 0xF;
			ECX = ByteLimt[ECX];
			EBP += ECX;// �ƺ��Ǵ�ģ�
			ESI += ECX;
			EAX >>= (ECX & 0xFF);
		}

		cout << "����0x80000000" << endl;
		if (EBP <= EDI) {
			do {
				if (EAX == EBX) {
					ESI += 4;
					EAX = 0x80000000;
				}
				uint8_t DL = encryptedData[ESI];
				// ���� EBP�±� �� DL д�� decryptedData
				memcpy(decryptedData.data() + EBP, &DL, sizeof(DL));
				EBP += EBX;
				ESI += EBX;
				EAX >>= 1;
			} while (EBP <= EDI);
		}

		cout << "��ɶ���" << endl;

		return decryptedData;
	}
}