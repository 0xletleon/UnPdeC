#include "Unpack.h"

namespace UnPdeC {
	// ��ʱ��������ν��ܵ��ļ�
	const vector<string> PassArr = { ".fsb", ".swf", ".ttf", "version", "language" };

	/// <summary>
	/// ���Խ���
	/// </summary>
	/// <param name="Offset">���ݿ���PDE�ļ��е�ƫ��ֵ</param>
	/// <param name="Size">���ݿ��С</param>
	/// <param name="Dir">Ŀ¼</param>
	/// <param name="Is170">�Ƿ�Ϊ170������</param>
	void Unpack::Try(uint32_t Offset, uint32_t Size, const DirStr& Dir, bool Is170) {
		std::cout << " �����ڳ��Խ���: " << Dir.NowDir << std::endl;

		//�������
		GetOffsetStr TryByte;
		vector<uint8_t> DeTryByte;

		try {
			// ��ȡ����
			TryByte = PdeTool::GetByteOfPde(Offset, Size);
			// У������
			//if (TryByte.Size != Size) return;
			// �������ݿ� -> ͬʱ����һ��������ʱʹ�õ�PDE�ļ�
			DeTryByte = PdeTool::DeFileOrBlock(TryByte.Byte);

			// �� DeTryByte ���浽��ǰ�ļ�����
			//string FileName = "Testx.hex";
			//std::ofstream ofs(FileName, std::ios::binary);
			//ofs.write(reinterpret_cast<const char*>(DeTryByte.data()), DeTryByte.size());
			//ofs.close();

			//std::cout << " �����ܳɹ�: " << FileName << std::endl;


		} catch (const std::exception& e) {
			std::cout << " ����ȡ����ʧ��: " << e.what() << std::endl;
			return;
		}

		// ��170������
		if (!Is170) {
			// ��ȡ�ļ����ļ���ƫ����Ϣ
			std::vector<HexOffsetInfo> DeTryByteArr = PdeTool::GetOffsetInfo(DeTryByte, Offset);
			// ��ȡ�������ļ���Ӳ��
			Save(DeTryByteArr, Dir, Offset);
		}

	}

	/// <summary>
	/// ����Ŀ¼���ļ�
	/// </summary>
	/// <param name="DirOrFileArr">�ļ���Ŀ¼����</param>
	/// <param name="Dir">Ŀ¼</param>
	/// <param name="BlockOffset">���ݿ���PDE�ļ��е�ƫ��ֵ</param>
	void Unpack::Save(const vector<HexOffsetInfo>& DirOrFileArr, const DirStr& Dir, uint32_t BlockOffset) {

		for (const HexOffsetInfo DirOrFile : DirOrFileArr) {
			if (DirOrFile.Type == 1) { // �ļ�
				// ��¼�ļ�ƫ����Ϣ
				//RecOffsetLog(blockOffset, DirOrFile.Offset, 0, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, dir.NowDir);

				// ��ȡָ��ƫ�Ƶ��ֽ�����
				GetOffsetStr TempFileByte = PdeTool::GetByteOfPde(DirOrFile.Offset, DirOrFile.Size);
				// У������
				if (TempFileByte.Size != DirOrFile.Size) break;

				// ��������
				std::vector<unsigned char> DeTempFileByte = PdeTool::DeFileOrBlock(TempFileByte.Byte);

				// �ж��Ƿ��ǿ��ļ�
				if (DeTempFileByte.empty() || DirOrFile.Name.empty()) break;

				// �����ļ�
				std::string savePath = Dir.NowDir + DirOrFile.Name;
				if (!std::filesystem::exists(savePath)) {
					// �����ļ�
					std::ofstream outFile(savePath, std::ios::binary);
					outFile.write(reinterpret_cast<const char*>(DeTempFileByte.data()), DeTempFileByte.size());
					outFile.close();
				}
			} else if (DirOrFile.Type == 2) { // Ŀ¼
				// ��¼Ŀ¼ƫ����Ϣ
				//RecOffsetLog(blockOffset, DirOrFile.Offset, 0, DirOrFile.Size, DirOrFile.Name, DirOrFile.Type, dir.NowDir);

				// ����Ŀ¼
				std::filesystem::create_directory(dir.NowDir + DirOrFile.Name + "/");
				// �ݹ����
				// Try(DirOrFile.Offset, DirOrFile.Size, new DirStr(...));
			} else {
				std::cout << "��������: " << DirOrFile.Name << std::endl;
			}
		}

	}

}