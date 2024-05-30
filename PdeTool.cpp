#include "PdeTool.h"

namespace UnPdeC {
	void PdeTool::Init() {
		cout << "���ڳ�ʼ�� PdeTool" << endl;

		// ���Ŀ¼�Ƿ����
		if (!std::filesystem::exists(GV::NowPde.Name)) {
			// Ŀ¼�����ڣ�������
			std::filesystem::create_directory(GV::NowPde.Name);
			std::cout << "�Ѵ���Ŀ¼: " << GV::NowPde.Name << std::endl;
		}

		// ��ȡPdeKey
		PdeKey::Get();

		cout << " �� �ɹ���ʼ��PDE������" << endl;
	}

	/// <summary>
	/// ��ȡ�ļ����ļ���ƫ����Ϣ
	/// </summary>
	/// <param name="data">���ν��ܺ������</param>
	/// <param name="BlockOffset">���ݿ���PDE�ļ��е�ƫ��ֵ</param>
	/// <returns>�ļ����ļ��е�ƫ����Ϣ���� </returns>
	std::vector<HexOffsetInfo> PdeTool::GetOffsetInfo(const std::vector<unsigned char>& data, uint32_t BlockOffset) {
		const int BlockSize = 128;
		size_t BlockCount = data.size() / BlockSize;
		std::vector<std::vector<uint8_t>> BlockArr(BlockCount);

		// ѭ���ֿ�
		for (int i = 0; i < BlockCount; ++i) {
			int start = i * BlockSize;
			int length = std::min(BlockSize, static_cast<int>(data.size()) - start);
			BlockArr[i].resize(length);
			std::copy(data.begin() + start, data.begin() + start + length, BlockArr[i].begin());
		}

		std::vector<HexOffsetInfo> OffsetInfoArr;

		// ѭ����ȡƫ��,��С,����,�ļ���
		for (int bi = 0; bi < BlockCount; ++bi) {
			const std::vector<uint8_t>& Block = BlockArr[bi];
			HexOffsetInfo ThisInfo{ 0, "", 0, 0, 0 };

			// ��ȡ���� 1==�ļ���2==Ŀ¼
			int NonZeroIndex = 0;
			for (int i = 0; i < Block.size(); ++i) {
				if (i == 0) {
					ThisInfo.Type = Block[i];
					// ��֤����
					if (ThisInfo.Type != 1 && ThisInfo.Type != 2) break;
				} else {
					// ��ȡ����
					int sub = Block[i] - 0x41;
					if (sub <= 25) sub += 32;
					if (sub <= 0) {
						NonZeroIndex = i - 1;
						break;
					}
				}
			}

			if (NonZeroIndex > 0) {
				// ��������
				ThisInfo.Name = std::string(Block.begin() + 1, Block.begin() + NonZeroIndex + 1);
				if (!NameValidator::Check(ThisInfo.Type, ThisInfo.Name)) {
					break;
				}
			} else {
				// ��ȡ�ļ�������
				break;
			}

			// ��ȡ��С
			// ʹ��C++17�Ĺ�����ֱ�ӳ�ʼ�����4���ֽ�
			std::vector<uint8_t> SizeBytes(Block.end() - 4, Block.end());
			union {
				uint32_t value;
				uint8_t bytes[4];
			} ThisSize{};
			// ֱ�ӽ��ֽڸ�ֵ��data��bytes���飬������ô���ֽ���
			std::copy(SizeBytes.begin(), SizeBytes.end(), ThisSize.bytes);

			// ����data.value�����˺ϲ����32λ��ֵ
			std::cout << "0x" << std::hex << ThisSize.value << std::endl;
			ThisInfo.Size = ThisSize.value;
			cout << "�ļ���: " << ThisInfo.Name << " ��С: " << ThisInfo.Size << " ����: " << (ThisInfo.Type == 1 ? "�ļ�" : "Ŀ¼") << endl;


			//// ��ȡԭʼƫ��ֵ
			//std::vector<uint8_t> ThisOffset(4);
			//std::copy(Block.end() - 8, Block.end() - 4, ThisOffset.begin());
			//ThisInfo.OOffset = static_cast<uint32_t>(ThisOffset[0]) |
			//	(static_cast<uint32_t>(ThisOffset[1]) << 8) |
			//	(static_cast<uint32_t>(ThisOffset[2]) << 16) |
			//	(static_cast<uint32_t>(ThisOffset[3]) << 24);

			// ʹ��C++17�Ĺ�����ֱ�ӳ�ʼ�����4���ֽ�
			std::vector<uint8_t> OOffsetBytes(Block.end() - 8, Block.end() - 4);
			union {
				uint32_t value;
				uint8_t bytes[4];
			} OOffset{};
			// ֱ�ӽ��ֽڸ�ֵ��data��bytes���飬������ô���ֽ���
			std::copy(OOffsetBytes.begin(), OOffsetBytes.end(), OOffset.bytes);

			// ����data.value�����˺ϲ����32λ��ֵ
			std::cout << "0x" << std::hex << OOffset.value << std::endl;
			ThisInfo.OOffset = OOffset.value;


			// ����ʵ��ƫ��ֵ
			ThisInfo.Offset = ((ThisInfo.OOffset >> 10) + ThisInfo.OOffset + 1) << 12;
			cout << "Offset: " << ThisInfo.Offset << endl;

			// ��� ThisInfo.Offset Խ�磬���˳�ѭ��
			if (ThisInfo.Offset >= GV::NowPde.Size) {
				std::cerr << "ThisInfo.Offset Խ���˳�ѭ��" << std::endl;
				continue;
			}

			//// ��ȡ��ʶ
			std::vector<uint8_t> TagBytes(Block.end() - 0x10, Block.end() - 0xc);


			//uint32_t TagOffSet = BlockOffset + ((bi + 1) * 0x80) - 0xC;
			//GetOffsetStr ThisTag = GetByteOfPde(TagOffSet, 4);
			//// ��ӡ ThisTag.Byte

			//// ��ӵ�OffsetInfoArr
			//OffsetInfoArr.push_back(ThisInfo);
		}

		return OffsetInfoArr;
	}

	/// <summary>
	/// ��PDE�ļ��л�ȡָ��������
	/// </summary>
	/// <param name="Start">����PDE�ļ��е���ʼƫ��</param>
	/// <param name="Size">���С</param>
	/// <returns>������</returns>
	GetOffsetStr PdeTool::GetByteOfPde(uint32_t Start, uint32_t Size) {
		// ���ļ����ڶ�ȡ����������
		ifstream PDEFILE_FS(GV::NowPde.Path, ios::binary);

		// ����ļ��Ƿ�򿪳ɹ�
		if (!PDEFILE_FS) {
			cerr << "�޷���PDE�ļ�: " << GV::NowPde.Name << endl;
			// �˳�����
			exit(1);
		}

		// ���ö�ȡ���
		PDEFILE_FS.seekg(Start, ios::beg);

		// ����ļ�λ���Ƿ���ȷ
		if (PDEFILE_FS.tellg() != Start) {
			cerr << "�ļ���λʧ��" << endl;
			return { 0, vector<uint8_t>() };
		}

		vector<uint8_t> buffer(Size);
		PDEFILE_FS.read(reinterpret_cast<char*>(buffer.data()), Size);

		if (!PDEFILE_FS) {
			cerr << "��ȡPDE�ļ�ʧ��" << endl;
			// �����ȡʧ�ܣ����ؿյ�GetOffsetStr
			return { 0, vector<uint8_t>() };
		}

		return { static_cast<uint32_t>(PDEFILE_FS.gcount()), buffer };
	}

	/// <summary>
	/// �����ļ� ��! ���ݿ�
	/// </summary>
	/// <param name="OffsetArr">�ļ������ݿ��ƫ����Ϣ����</param>
	/// <returns>���ܺ������</returns>
	std::vector<unsigned char> PdeTool::DeFileOrBlock(const std::vector<unsigned char>& OffsetArr) {
		// ��ǰ��ʱ�����ֽ�����
		std::vector<uint8_t> TempDEArr(OffsetArr.size(), 0);

		// Key����
		//size_t KeyLength = GV::PdeKey.size() - 1;
		// GV::PdeKey �Ĺ̶�����
		constexpr size_t KeyLength = 0x1000;

		// ���� OffsetArr ��ʹ�� GV::PdeKey ���н���
		for (size_t i = 0; i < OffsetArr.size(); ++i) {
			// ʹ��ģ����ȷ�� KeyIndex ѭ���� 0 �� KeyLength ֮��
			size_t KeyIndex = i % KeyLength;
			TempDEArr[i] = GV::PdeKey[KeyIndex] ^ OffsetArr[i];
		}

		// ��Կ����
		//size_t KeyIndex = 0;

		// ѭ������ÿ���ֽ�
		//for (size_t i = 0; i < OffsetArr.size(); ++i) {
		//	// ������
		//	uint8_t XorVal = GV::PdeKey[KeyIndex] ^ OffsetArr[i];
		//	TempDEArr[i] = XorVal;

		//	// ���KeyIndexԽ�磬�����¿�ʼ
		//	if (++KeyIndex >= KeyLength) {
		//		KeyIndex = 0;
		//	}
		//}

		// ��ɽ������ݿ�
		return TempDEArr;
	}

}