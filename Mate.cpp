#include "Mata.h"

namespace UnPdeC {
	/// <summary>
	/// ��ȡԪ��Ϣ
	/// </summary>
	/// <param name="MataBlock">������</param>
	/// <param name="XorFileSize">.xor�ļ���С</param>
	/// <returns>Ԫ��Ϣ</returns>
	BlockInfo Mata::ExtractMataInfo(const uint8_t* MataBlock, uint64_t XorFileSize) {
		// ��ȡ�����ļ���Ϣ
		BlockInfo blockInfo{};

		// �ж��ļ����Ƿ�Ϸ�
		if (MataBlock[0] != 0x01 && MataBlock[0] != 0x02) {
			return blockInfo;
		}

		// �ж��ļ����Ƿ�Ϸ�
		std::string name{ &MataBlock[1], std::find(&MataBlock[1], &MataBlock[0x50], '\0') };
		if (!Tools::NameCheck(name)) {
			return blockInfo;
		}

		// �жϴ������Ƿ�λ0x0
		if (!std::all_of(MataBlock + 0x50, MataBlock + 0x70, [](uint8_t c) { return c == 0x00; })) {
			return blockInfo;
		}

		// ��ȡ���жϱ�����ƫ��ֵ�Ƿ�Ϸ�
		uint64_t originalOffset = *reinterpret_cast<const uint32_t*>(&MataBlock[0x78]);
		if (originalOffset < 1) {
			return blockInfo;
		}

		// ��ȡ���ж��ļ���С�Ƿ�Ϸ�
		uint64_t size = *reinterpret_cast<const uint32_t*>(&MataBlock[0x7c]);
		if (size >= XorFileSize || size == 0) {
			return blockInfo;
		}

		// ��.xor�ж�ȡ�ļ���Ϣ
		struct BlockInfo {
			bool isValid = false; // ���Ƿ���Ч
			std::string type; // "1" for file, "2" for folder
			std::string name; // �ļ���
			std::string encodedOffset; // ������ƫ��ֵ
			std::string decodedOffset; // ������ƫ��ֵ
			std::string size; // ���ݿ��С
			std::string blockOffset; // ���ƫ��ֵ
			std::string blockEncodedOffset; // ��ı�����ƫ��ֵ
		};

		// ƴ����Ϣ
		blockInfo = {
			true,
			(MataBlock[0] == 0x01) ? "1" : "2",
			name,
			Tools::ToHexString(originalOffset),
			Tools::ToHexString(Tools::OrigToPdeOffset(originalOffset)),
			Tools::ToHexString(size),
			"",
			""
		};

		// ����Ԫ��Ϣ
		return blockInfo;
	}

	/// <summary>
	/// ��ȡ.xor������Ԫ���ݿ�
	/// </summary>
	/// <param name="FilePath">.xor�ļ�·��</param>
	/// <returns>�����ļ�/�е�Ԫ����json</returns>
	json Mata::ReadMatakForXorFile(const fs::path& FilePath) {
		std::cout << "\n ��Reading Mate information......\n";

		if (!fs::exists(FilePath)) {
			throw std::runtime_error("File does not exist: " + FilePath.string());
		}

		// ��ȡ�ļ�
		std::ifstream file(FilePath, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Unable to open file: " + FilePath.string());
		}

		file.seekg(0, std::ios::end);// ��ȡ�ļ���С
		uint64_t fileSize = file.tellg();// .xor�ļ���С
		file.seekg(0x1000, std::ios::beg);// �����ļ�ͷ 0x1000�Ǹ�Ŀ¼

		const uint64_t blockSize = 0x80;// ���С
		std::vector<uint8_t> buffer(blockSize);// ������

		json maps;// �洢Ŀ¼���ļ�ӳ����Ϣ
		json list;// �洢Ŀ¼���ļ�Ԫ��Ϣ

		uint64_t offsetIndex = 0x1000;// ������ƫ����

		while (file.read(reinterpret_cast<char*>(buffer.data()), blockSize)) {
			// ��ȡ����֤��
			BlockInfo BlockInfo = Mata::ExtractMataInfo(buffer.data(), fileSize);

			if (BlockInfo.isValid) {
				// ��ȡ��ǰƫ��ֵ��Ӧ�ı���ƫ��ֵ
				auto blockEncodedOffset = Tools::ToHexString(Tools::PdeToOrigOffset(offsetIndex));
				auto blockOffsetStr = Tools::ToHexString(offsetIndex);
				// �洢Ŀ¼���ļ�ӳ����Ϣ
				maps[Tools::ToHexString(Tools::PdeToOrigOffset(offsetIndex))].push_back(BlockInfo.encodedOffset);
				// �洢Ŀ¼���ļ�Ԫ��Ϣ
				list[BlockInfo.encodedOffset] = {
					{"type", BlockInfo.type},
					{"name", BlockInfo.name},
					{"encodedOffset", BlockInfo.encodedOffset},
					{"decodedOffset", BlockInfo.decodedOffset},
					{"size", BlockInfo.size},
					{"blockOffset", blockOffsetStr},
					{"blockEncodedOffset", blockEncodedOffset}
				};
			}
			// ��һ����
			offsetIndex += blockSize;
		}

		// �ϲ�JSON
		json output = {
			{"maps", maps},
			{"list", list}
		};

		// ����Ԫ��Ϣ������
		std::string outputPath = (GV::ExeDir / (GV::NowXor.Name + ".json")).string();
		std::ofstream outFile(outputPath);
		outFile << output.dump(4);
		outFile.close();

		std::cout << " << JSON: " << outputPath << "\n �� Meta information reading completed\n";

		// ����JSON
		return output;
	}

	/// <summary>
	/// ���벢�����ļ�/��
	/// </summary>
	/// <param name="MataJson">�����ļ���Ԫ��Ϣ</param>
	void Mata::DecodeAndSaveFile(const MataSaveInfo& MataJson) {
		// �ļ�
		if (MataJson.type == "1") {
			// ��ȡ�ļ�
			GetOffsetStr tempFileByte = Tools::GetByteOfXor(MataJson.offset, MataJson.size);
			// ����ļ���С
			if (tempFileByte.Size != MataJson.size) return;

			// Ensure directory exists
			fs::path dirPath = MataJson.directoryPath;
			if (!fs::exists(dirPath)) {
				fs::create_directories(dirPath);
			}

			// Check if the file name contains ".cache" to determine if it needs secondary decryption
			bool hasCache = MataJson.name.find(".cache") != std::string::npos;

			// ������ν���
			if (!hasCache) {
				// Save file
				fs::path filePath = dirPath / MataJson.name;
				if (!fs::exists(filePath)) {
					std::ofstream outFile(filePath, std::ios::binary);
					outFile.write(reinterpret_cast<const char*>(tempFileByte.Byte.data()), tempFileByte.Byte.size());
					outFile.close();
				}
			} else {
				// Secondary decryption
				std::vector<uint8_t> decryption2;
				std::string fixedName = MataJson.name;
				try {
					// ���ν���
					decryption2 = Decrypt::PreDecrypt(tempFileByte.Byte, fixedName);
					fixedName = MataJson.name.substr(0, MataJson.name.find(".cache"));
				} catch (const std::exception&) {
					std::cout << " ��Secondary decryption failed: " << MataJson.name << "\n";
					decryption2 = tempFileByte.Byte;
				}

				// Save file
				fs::path filePath2 = dirPath / fixedName;
				if (!fs::exists(filePath2)) {
					std::ofstream outFile(filePath2, std::ios::binary);
					outFile.write(reinterpret_cast<const char*>(decryption2.data()), decryption2.size());
					outFile.close();
				}
			}
		} else if (MataJson.type == "2") {
			// Create directory
			fs::path parentPath = MataJson.directoryPath;
			fs::path dirName = MataJson.name;
			fs::path dirPath = parentPath / dirName;
			if (!fs::exists(dirPath)) {
				fs::create_directories(dirPath);
				std::cout << " << Dir: " << dirPath << "\n";
			}
		} else {
			std::cout << " ��UNKONW:" << MataJson.name << "\n";
		}
	}

	/// <summary>
	/// ��ȡԪ��Ϣ������
	/// </summary>
	/// <param name="XorFileMatas">Xor�ļ�Ԫ��Ϣ</param>
	void Mata::ExtractMateAndDecode(const json& XorFileMatas) {
		std::cout << "\n ��Extracting Mate And Decode......\n\n";

		// ��������Mapӳ����Ϣ
		for (const auto& [key, values] : XorFileMatas.at("maps").items()) {
			if (values.empty()) {
				std::cout << " ��Empty map key: " << key << "\n";
				continue;
			}

			////////////////////////��ȡ��ƴ��Ŀ¼////////////////////////

			// ��ǰ·������
			std::vector<std::string> currentDirPath;

			// ��ȡ������Ŀ¼ֵ key -> "0x123"
			uint64_t blockOffsetHex = std::stoull(key.substr(2), nullptr, 16);

			// �ж��Ƿ��Ǹ�Ŀ¼ 0 �Ǹ�Ŀ¼(����ʹ��FastXor.html��֤)
			bool isRootDir = blockOffsetHex == 0;

			// �Ǹ�Ŀ¼ �� List�´��ڱ�����key
			if (!isRootDir && XorFileMatas.at("list").contains(key)) {
				// ��ȡ��ǰĿ¼����
				json currentDirObj = XorFileMatas.at("list")[key];

				// �ж��Ƿ�ΪĿ¼
				if (currentDirObj["type"] != "2") {
					std::cout << " ��Error! Not a directory\n";
					continue;
				}

				// ��ȡ��ǰĿ¼���� -> currentDirPath����
				currentDirPath.push_back(currentDirObj["name"]);
				// ��ȡ16���Ƶı�����ƫ��ֵ
				uint64_t upDirBlockOOffsetHex = std::stoull(currentDirObj["blockEncodedOffset"].get<std::string>(), nullptr, 16);

				// ��� > 0 ���ʾ�����ϼ�Ŀ¼
				while (upDirBlockOOffsetHex > 0) {
					// ��ȡ�ϼ�Ŀ¼�ı���ƫ��ֵ
					std::string upOffset = currentDirObj["blockEncodedOffset"];

					// �ж��Ƿ����
					if (XorFileMatas.at("list").contains(upOffset)) {
						// ����
						// ��ȡ�ϼ�Ŀ¼����
						currentDirObj = XorFileMatas.at("list")[upOffset];
						// ��ȡ�ϼ�Ŀ¼���� -> currentDirPath����
						currentDirPath.push_back(currentDirObj["name"]);
						// ��ȡ�ϼ�Ŀ¼�ı���ƫ��ֵ
						upDirBlockOOffsetHex = std::stoull(currentDirObj["blockEncodedOffset"].get<std::string>(), nullptr, 16);
					} else {
						// ��������ŵ� Other Ŀ¼��
						currentDirPath.push_back("Other");
						break;
					}
				}
			} else {
				// �Ǹ�Ŀ¼
				if (!isRootDir) {
					// ����ƫ��ֵ -> currentDirPath����
					currentDirPath.push_back(key);
					// ����OtherĿ¼
					currentDirPath.push_back("Other");
				}
			}

			// ��ǰ���ڴ����.xor�ļ���(�޺�׺) -> currentDirPath����
			currentDirPath.push_back(GV::NowXor.Name);
			if (currentDirPath.size() > 1) {
				// ��ת
				std::reverse(currentDirPath.begin(), currentDirPath.end());
			}

			// ƴ�Ӻ��·��
			std::string dirTempPath;
			for (const auto& path : currentDirPath) {
				dirTempPath += path + "\\";
			}

			// ƴ�Ӻ�ľ���·��
			std::string filePath = (GV::ExeDir / dirTempPath).string();
			std::cout << " Dir: " << dirTempPath << "\n";

			////////////////////////�������뱣��Ŀ¼�µ��ļ�////////////////////////

			// ����Ŀ¼�µ��ļ�
			for (const auto& value : values) {
				// �ж��Ƿ����
				if (XorFileMatas.at("list").contains(value)) {
					// ��ȡ��Ϣ
					json info = XorFileMatas.at("list")[value];
					// ��ȡ��Ϣ
					MataSaveInfo saveInfo{
						info["type"],
						info["name"],
						std::stoull(info["decodedOffset"].get<std::string>(), nullptr, 16),
						std::stoull(info["size"].get<std::string>(), nullptr, 16),
						filePath
					};
					// ���벢�����ļ�
					Mata::DecodeAndSaveFile(saveInfo);
				}
			}
		}
		std::cout << "\n ��Extracting Mate And Decode completed!\n\n";
	}
}